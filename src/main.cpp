#include <mbed.h>
#include <math.h>
#include <USBSerial.h>
#include "LIS3DSH.h"

#define PI 3.1415926

USBSerial serial;
LIS3DSH acc(SPI_MOSI, SPI_MISO, SPI_SCK, PE_3);
DigitalOut LED_situp(LED1);
DigitalOut LED_pushup(LED2);
DigitalOut LED_squat(LED5);
DigitalOut LED_jump(LED6);
DigitalIn button(USER_BUTTON);

int16_t X = 0;             // accelerate for X axis
int16_t Y = 0;             // accelerate for Y axis
int16_t Z = 0;             // acceleration for Z axis (raw)
uint8_t ringbuf_index = 0; //index to insert sample
const uint8_t N = 10;      // filter length
float ringbuf[N];          // sample buffer
float g_z = 0;             // acceleration for Z axis (G force)
float angle = 0;           // angle relative to Z axis

int status_situp = 0;
int status_pushup = 0;
int status_squat = 0;
int status_jump = 0;
int count_situp = 0;
int count_pushup = 0;
int count_squat = 0;
float count_jump = 1.0;

typedef enum state_t{
    lying,
    sitting,
    lying_down //lying_down is facing to the ground
} state_t;

state_t state = lying;

static void state_find(){
    switch (state){
        case lying:
            if (angle > 45 && angle < 135)
                state = sitting;
            else if (angle > 135)
                state = lying_down;
            break;

        case sitting:
            if (angle < 45)
                state = lying;
            else if (angle > 135)
                state = lying_down;
            break;

        case lying_down:
            if (angle < 45)
                state = lying;
            else if (angle > 45 && angle < 135)
                state = sitting;
            break;

        default:
            printf("There is a mistake\n\r");
    }
}

typedef enum work_order{
    situps,
    squats,
    jumping_jacks,
    pushups,
    detecting
} work_order;

work_order work = detecting;

static void exercise_detect(){
    if (state == lying)
        work = situps;
    else if (state == lying_down)
        work = pushups;
    else{
        work = squats;
    }
//I will determine if it is squat or jump jack while we are doing the exercise
}

static void LED_flash(){
    LED_situp = 1;
    LED_squat = 1;
    LED_jump = 1;
    LED_pushup = 1;
    wait_ms(500);
    LED_situp = 0;
    LED_squat = 0;
    LED_jump = 0;
    LED_pushup = 0;
}

static float calc_angle(){
    g_z = (float)Z / 17694.0;
    ringbuf[ringbuf_index++] = g_z;
    if (ringbuf_index >= N)
        ringbuf_index = 0;
    float g_z_filt = 0;
    for (uint8_t i = 0; i < N; i++){
        g_z_filt += ringbuf[i];
    }
    g_z_filt /= (float)N;
    if (g_z_filt > 1)
        g_z_filt = 1;
    angle = 180 * acos(g_z_filt) / PI;
    return angle;
}

static void situp(){
    switch (status_situp){
        case 0:
            if (state == lying)
                status_situp = 1;
            break;
        case 1:
            if (state == sitting){
                status_situp = 0;
                count_situp++;
            }
            break;
        default:
            break;
    }
}

static void squat(){
    if (Y <= -30000){
        work = jumping_jacks;
        status_squat = 0;
    }
    switch (status_squat){
        case 0:
            if (Y < -20000)
                status_squat = 1;
            break;
        case 1:
            if (Y > -20000){
                status_squat = 0;
                count_squat++;
            }
            break;
        default:
            break;
    }
}

static void jump(){
    switch (status_jump){
        case 0:
            if (Y > 0)
                status_jump = 1;
            break;
        case 1:
            if (Y < -10000){
                status_jump = 0;
                count_jump += 0.5;
            }
            break;
        default:
            break;
    }
}

static void pushup(){
    switch (status_pushup){
    case 0:
        if (Z > -12000)
            status_pushup = 1;
        break;
    case 1:
        if (Z <= -15000){
            status_pushup = 0;
            count_pushup++;
        }
        break;
    default:
        break;
    }
}

static void position(){
    switch (work){
    case situps:
        situp();
        if (count_situp == 15)
            LED_situp = 1;
        break;
    case squats:
        squat();
        if (count_squat == 15)
            LED_squat = 1;
        break;
    case jumping_jacks:
        jump();
        if (count_jump == 15)
            LED_jump = 1;
        break;
    case pushups:
        pushup();
        if (count_pushup == 15)
            LED_pushup = 1;
        break;
    default:
        break;
    }
}

int main(){
    while (acc.Detect() != 1){
        printf("Could not detect Accelerometer\n\r");
        wait_ms(200);
    }

    LED_situp = 0;
    LED_squat = 0;
    LED_jump = 0;
    LED_pushup = 0;

    while (1){
        acc.ReadData(&X, &Y, &Z);
        angle = calc_angle();
        state_find();
        position();
        if (button == 1)
            exercise_detect();
        if (count_situp >= 15 && count_squat >= 15 && count_jump >= 15 && count_pushup >= 15)
            LED_flash();
        wait_ms(100);
    }
}