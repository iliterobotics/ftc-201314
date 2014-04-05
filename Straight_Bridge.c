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

/*
Manipulator included:
* Four drivetrain motors wired to two ports
* One servo for the flikcer (autonomous scoring mechanism)
* One IR seeker for finding the IR beacon

*/

#include "JoystickDriver.c"


short ENCODER = dt_left;

const int IR_SEEKING_VEL = 30;
const int LAST_BUCKET_DIST = 40;
const int TOTAL_BRIDGE_DIST = 65;
const int ARM_EXTENDED_POS = 0;
const int SERVO_STOP_TIME = 2000;
const int ARM_RETRIEVED_POS = 127;
const int MAX_VEL = 80;
const int FIRST_TURN_DEGREE = -75;
const int CENTER_BRIDGE_DIST = 50;
const int SECOND_TURN_DEGREE = -130;
const int RAMP_DIST = 55;

const int ROBOT_DIAMETER = 20;
const int TURN_VEL = 60;


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
	const int turn_dist = ROBOT_DIAMETER * PI * p_degrees / 360;
	int direction = p_degrees / abs(p_degrees);

	do {
		motor[dt_left] = -TURN_VEL * direction;
		motor[dt_right] = TURN_VEL * direction;
		writeDebugStream("turnDegrees: %d, %d", nMotorEncoder[ENCODER], inchesToEncoder(turn_dist));
	} while(abs(nMotorEncoder[ENCODER]) < inchesToEncoder(abs(turn_dist)));

	moveDT(0);
}

task main() {
	initializeRobot();
	waitForStart(); // Wait for the beginning of autonomous phase.

	// Declares an enumeration defines all phases/states of the autonomous period
	typedef enum {
		kStart = 0,
		kGetOnRamp,
		kFinishAuto
	} State;
	// Declares a State variable to iterate through all phases/states
	State m_state = kStart;
	servo[auto_block] = 255;
	// Main state machine
	while(m_state != kFinishAuto) {
		writeDebugStream("State %d: ", m_state);
		switch(m_state) {

			// To fix the bug where the flicker actuates immediately without moving forward if the beacon is above the first bucket.
		case kStart:
			moveDT(IR_SEEKING_VEL);
			m_state++;
			break;

			// Move forward until IR seeker centers the IR beacon

			// Move straight forward a certain number of encoder ticks
		case kGetOnRamp:
			writeDebugStream("kGetOnRamp - %d", nMotorEncoder[ENCODER]);
			if(abs(nMotorEncoder[ENCODER]) < inchesToEncoder(RAMP_DIST)) {
				moveDT(MAX_VEL);
				} else {
				moveDT(0);
				nMotorEncoder[ENCODER] = 0;
				m_state++;
			}
			break;

			// Indicates the end of the autonomous program
		default:
			break;
		}

		writeDebugStreamLine("");
	}

	// "Special" RobotC thing where it needs a forever loop at the end of the autonomous program to wait for the end of the auonomous period
		for(;/*ever*/;) {}
}
