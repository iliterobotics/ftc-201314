#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Hubs,  S2, HTServo,  none,     none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     ,               sensorI2CMuxController)
#pragma config(Sensor, S4,     ir_seeker,      sensorHiTechnicIRSeeker600)
#pragma config(Motor,  mtr_S1_C1_1,     dt_left,       tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     dt_right,      tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_1,     archemedes,    tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     flag_raiser,   tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S2_C1_1,    auto_block,           tServoStandard)
#pragma config(Servo,  srvo_S2_C1_2,    bucket,               tServoStandard)
#pragma config(Servo,  srvo_S2_C1_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S2_C1_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Pragma updated Feb 8 to remove the second bucket servo, hookhold servo and move the bucket servo to port 2

#include "JoystickDriver.c"

short ENCODER = dt_left;

int MAX_POWER = 80;
int MOVE_DIST_1 = 30;
int TURN_DEGREES = 60;
int MOVE_DIST_2 = 40;

void initializeRobot() {
	// Make sure all motors are not moving before waitForStart()
	motor[dt_left] = 0;
	motor[dt_right] = 0;
	nMotorEncoder[ENCODER] = 0;
	motor[archemedes] = 0;
	motor[flag_raiser] = 0;

		servo[auto_block] = 255;
  	servoChangeRate[auto_block] = 0;

  	servo[bucket] = 255;
  	servoChangeRate[bucket] = 0;

	return;
}

void moveDT(int a_speed) {
	motor[dt_right] = a_speed;
	motor[dt_left] = a_speed;

}

int inchesToEncoder(float p_inches) {
	// Calibrated December 11, 2013
	return 140.74 * p_inches - 254.72;
	/* return p_inches * TICKS_PER_ROTATION / WHEEL_CIRCUM; */
}

void turnDegrees(int p_degrees) {
	const int ROBOT_DIAMETER = 20;
	const int TURN_VEL = 60;

	const int turn_dist = ROBOT_DIAMETER * PI * p_degrees / 360;
	int direction = p_degrees / abs(p_degrees);

	nMotorEncoder[ENCODER] = 0;

	do {
		motor[dt_left] = -TURN_VEL * direction;
		motor[dt_right] = TURN_VEL * direction;
		writeDebugStream("turnDegrees: %d, %d", nMotorEncoder[ENCODER], inchesToEncoder(turn_dist));
	} while(abs(nMotorEncoder[ENCODER]) < inchesToEncoder(abs(turn_dist)));

	moveDT(0);
}

void moveInches(int p_inches) {
	const int encoder_dist = inchesToEncoder(p_inches);

	nMotorEncoder[ENCODER] = 0;
	do {
		moveDT(MAX_POWER);
		writeDebugStreamLine("%d, %d", nMotorEncoder[ENCODER], encoder_dist);
	} while(abs(nMotorEncoder[ENCODER]) < abs(encoder_dist));
	moveDT(0);
}

task main() {
	initializeRobot();
	waitForStart(); // Wait for the beginning of autonomous phase.

	servo[auto_block] = 255;
	// Move forward
	writeDebugStream("move 1: ");
	moveInches(MOVE_DIST_1);

	// Turn right
	writeDebugStream("turn: ");
	turnDegrees(TURN_DEGREES);

	// Move forward onto the bridge
	writeDebugStream("move 2: ");
	moveInches(MOVE_DIST_2);

	// "Special" RobotC thing where it needs a forever loop at the end of the autonomous program to wait for the end of the auonomous period
		for(;/*ever*/;) {}
}
