
#pragma config(Sensor, in3,    lineFollowerLEFT,   sensorLineFollower)
#pragma config(Sensor, in2,    lineFollowerRIGHT,    sensorLineFollower)
#pragma config(Sensor, in1,    lineFollowerMIDDLE,  sensorLineFollower)
#pragma config(Sensor, dgtl9,  rightSensor,    sensorSONAR_cm)
#pragma config(Sensor, dgtl11, leftSensor,    sensorSONAR_cm)
#pragma config(Motor,  port1,           LeftMotor,     tmotorVex393_HBridge, openLoop, driveLeft)
#pragma config(Motor,  port10,          RightMotor,    tmotorVex393_HBridge, openLoop, driveRight)
#pragma config(Motor,  port6,           Intake,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           Feeder,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           ShooterMotorB, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           ShooterMotorA, tmotorVex393_MC29, openLoop)
//The following code above is used to assign motors and sensors to onboard ports on the vex cortex

// The following task(s) below allow for multi-threading various movements without "hogging" the CPU and slowing down the drive motors
task rampUpShooter(){
	while(true)
	{
		motor (ShooterMotorA) = 127;
		motor (ShooterMotorB) = 127;
	}
}
task rampDownShooter{
	while (true)
	{
		motor (ShooterMotorA) = 0;
		motor (ShooterMotorB) = 0;
	}
}
task startIntake()
{
	while (true)
	{
		motor (Intake) = 127;
		motor (Feeder) = 127;
	}

}
task stopIntake()
{
	while (true)
	{
		motor (Intake) = 0;
		motor (Feeder) = 0;
	}

}
task reverseIntake()
{
	while (true)
	{
		motor (Intake) = -127;
		motor (Feeder) = -127;
	}

}
// The task(s) above allow for the start and stop toggle of the shooter and the belt intake

task autonomous()
{
	// Start of line following code
	while (true)
	{

		if (SensorValue(lineFollowerRIGHT) > 1800)
		{
			motor(LeftMotor) = 0;
			motor(RightMotor) = -40; // if the right sensor sees black the robot is shiftted so that the left sensor can see black
		}

		if (SensorValue(lineFollowerLEFT) > 1800)
		{
			motor(RightMotor) = 0;
			motor(LeftMotor) = -40; // if the left sensor sees black the robot is shiftted so that the right sensor can see black
		}

		if ((SensorValue(lineFollowerLEFT) > 1800) && (SensorValue(lineFollowerRIGHT) > 1800) && (SensorValue(lineFollowerMIDDLE) > 1800)) // if all 3 sensors see black the robot is at the end of the line following track
		{
			// the loop is used to bring the robot 40 cm aways from the barier wall
			// the loop checks with both sensors to assure that both sides are seing a barrier
			while ((SensorValue(leftSensor) > 40) && (SensorValue(rightSensor) > 40))
			{
				motor(LeftMotor) = -40;
				motor(RightMotor) = -40;
				// motors are moved till loop satisfied
			}
			motor(LeftMotor) = 0;
			motor(RightMotor) = 0;
			break;
			// motors are stopped and the parent while loop is broken to advance to the next segment of autonomous
		}
	}
	// Start of algorithim to make robot parallel to the wall
	while (true)
	{
		if (SensorValue(rightSensor) > SensorValue(leftSensor))
		{
			motor(LeftMotor) = -40;
			motor(RightMotor) = 40;
			// if the right sensor is further away the right side is moved closer
		}
		if (SensorValue(leftSensor) > SensorValue(rightSensor))
		{
			motor(LeftMotor) = 40;
			motor(RightMotor) = -40;
			// if the left sensor is further away the left side is moved closer
		}
		if ((SensorValue(leftSensor) == SensorValue(rightSensor) - 1) || (SensorValue(rightSensor) == SensorValue(leftSensor) - 1))
		{
			motor(LeftMotor) = 0;
			motor(RightMotor) = 0;
			break;
			// motors are stopped when both sensors see a value within 1 cm of each other the robot is now parallel
		}
		motor(LeftMotor) = -200;
		motor(RightMotor) = 200;
		wait1Msec(700);
		motor(LeftMotor) = 0;
		motor(RightMotor) = 0;
		// motors are started to rotate the robot 180 degrees to make the shooter parallel with the net(s)

	}










}


// The main task is the block which contains the code that is initilized and running on the CPU
task main()
{

	int throttle; //  An integer to store the values of a joystick controlling the forwards and backwards

	int pitch; // Follows the same principal as the throttle integer but stores values for the left and right control

	int shooterStart; // An integer to store the values of the button controlling the shooter

	int shooterStop;

	bool autonomousStatus; // boolean assigned to store value of current state of autonomous

	autonomousStatus = false; // initial status of autonomus is off so flase state is assigned


	while (true) {
		pitch = vexRT[Ch4];
		throttle = vexRT[Ch2];
		shooterStart = vexRT[Btn6D];
		shooterStop = vexRT[Btn5D];
		// following code above is used to assign Vex Contoller button values to variables to be easily accessed

		if (pitch > 0) {
			motor(LeftMotor) = (throttle + pitch);
			motor(RightMotor) = (throttle - pitch); // this if statment is used to identify the change in stick position to determine left and right along with speed
		}
		else if (pitch < 0) {
			motor(LeftMotor) = (throttle + pitch);
			motor(RightMotor) = (throttle - pitch); // this if statment follows the same logic but when the stick is in a diffrent direction
		}
		else {
			motor(LeftMotor) = throttle;
			motor(RightMotor) = throttle;
		}
		// the block of code above code above calculates how much power to put in each wheel depending on the conditions and input

		if (shooterStart == 1) {
			stopTask(rampDownShooter); // this stops the shooter to make sure the shooter is not damaged when moving in reverse
			startTask(rampUpShooter); // this starts the shooter
		} // the following block is a routine to start the shooter

		if (shooterStop == 1) {
			stopTask(rampUpShooter); //
			startTask(rampDownShooter);
		}// the following block is a routine to stop the shooter

		if (vexRT[Btn8D] == 1) {
			stopTask(stopIntake);
			stopTask(reverseIntake);
			startTask(startIntake);
		}

		if (vexRT[Btn8R] == 1) {
			stopTask(startIntake);
			stopTask(reverseIntake);
			startTask(stopIntake);
		}

		if (vexRT[Btn8L] == 1) {
			stopTask(startIntake);
			stopTask(stopIntake);
			startTask(reverseIntake);
		}
		// the following 3 if statments are to start, stop and reverse the intake belt respectively

		/*if (placeHolderButton == 1) // if button is placed to toggle autonomous, following logic is ran
		{
			if (autonomousStatus == false) // if auto is off it is started and the status has changed
			{
				startTask(autonomous);
				autonomousStatus = true;
			}

			if (autonomousStatus == true) // if auto is on it is turned off and the status has changed
			{
				stopTask(autonomous);
				autonomousStatus = false;
			}
		}*/
	}



}


// All code created by Zarvaan Bacha