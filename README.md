# Welcome to use the Embedded fitness program

- Embedded fitness is a function that could help make a workout routine
- Embedded board: STM32F4D
- Creator: Wentai Liu
- Video demo: https://drive.google.com/file/d/1MOkxBOwM9-z1aMGxkAyXoo6T-zQzYxMh/view?usp=sharing

## Program Overview
	The program could support 4 different kinds of exercises, which are squats, pushups, situps, and jumping jacks. 
	As the default function usage, the number of each group of exercises is set as 15.
	There are four LEDs on the STM32F4D board which indicate the finishing status of each exercise and all of them will go flashing when the workout routine has been completed.
	The user needs to tie the board in front of the chest for workout detection and the 3.5mm audio input port should point to the ground(point down to the body).
	Before each new exercise, the user needs to push the blue button to begin.
	Suggests the user do squats before jumping jacks to avoid 1 more count for squats.
    Before the first exercise for each start, wait for 2s before pushing the button to start doing exercises.

## Function explanation
### state_find()
This function is used for detecting the user's body statement. 
There are 3 statuses which are lying, sitting, and lying_down. These states could be used for dividing different exercises' start positions.
- **Lying**: The user is lying on the ground and facing above the sky.
- **Sitting**: The user is standing and the body is vertical to the ground.
- **Lying_down**: The user is lying down to the ground and facing to the ground.

### exercise_detect()
This function is used to detect the correct exercise automatically.
Each time the user wants to do a different exercise, the user needs to press the button and the function will check the statement of the user's body and find out which exercise the user wants to do.
- If the user is **lying**, the exercise the user wants to do is situps.
- If the user is **lying_down**, the exercise the user wants to do is pushups.
- If the user is **sitting**, the exercise the user wants to do is squats or jumping jacks.
To divide the difference between the squats and jumping jacks, the function is set to default doing squats and the dividing in the **squat()** function later.

### LED_flash()
This function is used for LED flashing after all exercises are finished.
All LEDs will blink at around 0.5ms time intervals.

### calc_angle()
This function is used to calculate the angle of the body.
- Use the buffer to store the data of processed Z-axis data
- Add the data from the buffer and calculate the average number
  - In this way, the data will be more evenly and reduce errors
- Use the angle formula to transfer the radian to the degree

### situp()
This function is used to count the number of situps that are finished.
Every time the user does an exercise from lying, it will be a situp movement and each status change from lying to sitting will count as one situp.

### squat()
This function is used to count the number of squats that are finished and determine if the user wants to do squats or jumping jacks.
Every time the user does an exercise from sitting, it will be a squat movement and read the data change of the Y-axis for counting.
- If the Y-axis changes too much, the user is doing a jumping jack.
  - Programmer tests is Y-axis data for each jumping jack exercise will reach down to -30000.
  - If the user is doing a jumping jack, change the workout status to **jumping_jacks**

### jump()
This function is used to count the number of jumps that are finished.
The jumping jacks need 2 jumps for each action, so the function will count 0.5 for each jump, which means if the action is not correct, it will just count the correct half jump.

### pushup()
This function is used to count the number of pushups that are finished.
Every time the user does an exercise from lying_down, it will be a pushup movement and use the Z-axis data change for counting pushups.

### position()
This function is used for calling all other workout functions.
- Use the switch case to determine which exercise the user is doing right now.
- Call the corresponding workout function for exercise counting.
- Use the **if()** function to check if the user finishes the estimated exercise.
- The corresponding LED will be set as 1 (turn on) to show the user has finished the corresponding exercise.

### main()
The main function is used only for calling all other functions.
- The first while loop is used for detecting the error of the accelerometer, the program will run when the accelerometer is working successfully.
- All four LEDs that indicate each exercise will be set as 0 at the beginning of the workout.
- The second while loop is running all the time for continuous running of the function
 - The **ReadData()** will record the data from the accelerometer as X-axis, Y-axis, and Z-axis
 - Call the **calc_angle()** in order to calculate current angle status
 - Call the **state_find()** in order to find a current position by using the angle
 - Call the **position()** in order to check the workout status, do the count, and determine if the exercise reaches the estimation
 - Use the **if()** function to check if the user wants to do another exercise
   - Call the **exercise_detect()** in order to check which exercise the user wants to do
 - Use the **if()** function to check if the user has finished all exercises
   - Call the **LED_flash()** in order to prompt the user that a set workout has been done
 - Use the **wait_ms()** to set the frequency of the whole program

## Program result analysis
	The function not only analysis the number of the exercises the user has done, it could also detect if the user is not doing the workout uncorrectly.
	In the tests the programmer did, only the pushups will have around 1 deviation which because the pushups moves body in a not visible angle or Z axis change.
	If the user did jumping jacks before squats, the counting squat will count 1 more due to moving range of jumping jack is larger than the moving range of squat and when determine the exercise, squat will change status. When doing squats after jumping jacks, it will automatically change status of squat and count 1 more.
    Because the angle needs around 1s - 2s for calibration when it starts to read the data from accelerometer. Using the button for exercise starting could let all other functions work and the angle do its calibration before exercising. 
