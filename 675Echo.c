#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    gyroSensor,     sensorGyro)
#pragma config(Sensor, in4,    flyBattery,     sensorAnalog)
#pragma config(Sensor, dgtl1,  brakePistons,   sensorDigitalOut)
#pragma config(Sensor, dgtl2,  lockPistons,    sensorDigitalOut)
#pragma config(Sensor, dgtl3,  deployerPiston, sensorDigitalOut)
#pragma config(Sensor, dgtl4,  laser,          sensorDigitalOut)
#pragma config(Sensor, dgtl5,  ,               sensorDigitalOut)
#pragma config(Sensor, dgtl6,  ,               sensorDigitalOut)
#pragma config(Sensor, dgtl7,  yellowLED,      sensorDigitalOut)
#pragma config(Sensor, dgtl8,  greenLED,       sensorDigitalOut)
#pragma config(Sensor, dgtl9,  redLED,         sensorDigitalOut)
#pragma config(Sensor, dgtl10, intakeTouchSensor, sensorTouch)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           leftback,      tmotorVex393HighSpeed_HBridge, openLoop)
#pragma config(Motor,  port2,           topLeftLauncher, tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_4)
#pragma config(Motor,  port3,           bottomLeftLauncher, tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           topRightLauncher, tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port5,           bottomRightLauncher, tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           roller,        tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           chain,         tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           leftfront,     tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port9,           rightfront,    tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port10,          rightback,     tmotorVex393HighSpeed_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
bool userControl = false;
#pragma platform(VEX)
#pragma competitionControl(Competition)
#include "cool.c"
#include "FlyWheelAndIntake.h"
#include "jpearmanTBH.h"
#include "DriveAndPneumatics.h"
#include "SpeakerAndLCD.h"

bool usePID = true;

void pre_auton()
{
    SensorValue[laser] = 1;
		bStopTasksBetweenModes = false;
		SensorType[in1] = sensorNone;
  	wait1Msec(1000);
  	SensorType[in1] = sensorGyro;
  	wait1Msec(1000);
		startTask(LCD);
}

task autonomous()
{
    SensorValue[laser] = 0;
	runAuto(chosenAuto);
}

task usercontrol()
{
	SensorValue[laser] = 0;
	userControl = true;
	rpmGoal = rpmHigh;
	clearDebugStream();
	if(getTaskState(LCD) == taskStateStopped)
		startTask(LCD);
	if(usePID) {
		if(getTaskState(flyWheelPower) == taskStateStopped)
			startTask(flyWheelPower);
		if(getTaskState(flyWheelControl) == taskStateStopped)
			startTask(flyWheelControl);
	} else {
		if(getTaskState(tbh) == taskStateStopped)
			startTask(tbh);
	}
	if(getTaskState(drive) == taskStateStopped)
		startTask(drive);
	if(getTaskState(pneumatics) == taskStateStopped)
		startTask(pneumatics);
	if(getTaskState(speaker) == taskStateStopped)
		startTask(speaker);
	if(getTaskState(intake) == taskStateStopped)
		startTask(intake);
}
