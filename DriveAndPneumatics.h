#define lock SensorValue[lockPistons]
#define deployer SensorValue[deployerPistons]
#define brake SensorValue[brakePistons]
#define gyro SensorValue[gyroSensor]
#define transmission SensorValue[gyroSensor]
#define leftDriveEnc nMotorEncoder[leftDrive]
#define rightDriveEnc nMotorEncoder[rightDrive]
#define accelX SensorValue[accelerometerX]
#define accelY SensorValue[accelerometerY]
#define rampBtn vexRT[Btn7D]
#define rampBtn2 vexRT[Btn7L]
#define brakeBtn vexRT[Btn5U]
#define transBtn vexRT[Btn7D]
#define puncherBtn vexRT[Btn6U]

bool lifted = false;
bool braking = false;
bool punchersActivated = false;
int threshold = 15;
int liftCount = 0;
int brakePower = 25;
int brakeTime = 35;
int brakeCount = 0;

task drive();
task pneumatics();
void drivePower(int left, int right);
void driveBrake(int direction);
void releaseBrake();
void actuateBrake();
void releaseLift();
void lockLift();
void shiftTransmission();
void deploy();
void encoderTurn(int goal);
void gyroTurn(int goal);
void gyroTurnTo(int goal, int direction);
void transPower(int left, int right);

task drive()
{
	while(true) {

		drivePower(vexRT(Ch3), vexRT(Ch2));

    if(punchersActivated) {
        if(puncherBtn) {
      		 transPower(127, 127);
    		} else {
       		 transPower(0,0);
    		}
    }
	}
}

task pneumatics()
{
	bool lastBrakeBtn = false;
	bool lastRampBtn = false;
	bool lastTransBtn = false;
	deployer = 0;
	lockLift();
	while(true) {
		/* Ramp */
		if(rampBtn == 1 && rampBtn2 == 1 && lastRampBtn == false) {
			if(liftCount == 0) {
				deploy();
				liftCount++;
			} else if(liftCount == 1) {
				releaseLift();
				lifted = true;
				liftCount++;
			} else {
				lockLift();
				liftCount = 0;
			}
			lastRampBtn = true;
		} else if (rampBtn == 0) {
			lastRampBtn = false;
		}

		/* Brakes */
		if(brakeBtn == 1 && lastBrakeBtn == false) {
			if(braking)
				releaseBrake();
			else
				actuateBrake();
			lastBrakeBtn = true;
		} else if (brakeBtn == 0) {
			lastBrakeBtn = false;
		}

        /* Puncher Activation */
		if(transBtn == 1 && !lastTransBtn)
            shiftTransmission();
        else
            lastTransBtn = false;

	}
}

void shiftTransmission()
{
    transmission = (punchersActivated) ? 0 : 1;
    punchersActivated = !punchersActivated;
}

void deploy()
{
	deployer = 1;
	wait1Msec(400);
	deployer = 0;
}

void lockLift()
{
	lock = 0;
}

void releaseLift()
{
	lock = 1;
}

void actuateBrake()
{
	if(!braking) {
		brake = 1;
		braking = true;
		brakeCount++;
	}
}

void releaseBrake()
{
	brake = 0;
	braking = false;
}

void driveDistance(int goal)
{
    float driveKp = 0.35;
    float driveKd = 0.0;

    int leftDriveError;
    int leftDriveLastError;
    int leftDriveDerivative;
    int rightDriveError;
    int rightDriveLastError;
    int rightDriveDerivative;
    int maxPower = 105;
    int allowableError = 0;

    leftDriveEnc = 0;
    rightDriveEnc = 0;
    clearTimer(T1);

    while(!(abs(leftDrive) > abs(goal) - allowableError) || !(abs(rightDrive) > abs(goal) - allowableError)){
    		if(userControl)
    			break;
        if(time1[T1] > abs(goal) * 3)
            break;

        // Proportional
        leftDriveError = (abs(goal) - abs(leftDrive));
        rightDriveError = (abs(goal) - abs(rightDrive));

        // Derivative
        leftDriveDerivative = leftDriveError - leftDriveLastError;
        rightDriveDerivative = rightDriveError - rightDriveLastError;
        leftDriveLastError = leftDriveError;
        rightDriveLastError = rightDriveError;

        // PID
        int leftPidDrive = round(((driveKp * leftDriveError) + (driveKd * leftDriveDerivative)));
        int rightPidDrive = round(((driveKp * rightDriveError) + (driveKd * rightDriveDerivative)));
        leftPidDrive = (abs(leftPidDrive) > maxPower) ? maxPower : leftPidDrive; // limit to a maxPower
        rightPidDrive = (abs(rightPidDrive) > maxPower) ? maxPower : rightPidDrive;

        leftPidDrive = (goal < 0) ? -leftPidDrive : leftPidDrive;
        rightPidDrive = (goal < 0) ? -rightPidDrive : rightPidDrive;

        drivePower(leftPidDrive, rightPidDrive);
    }
    driveBrake(goal);
    // check for overshoot
}

void encoderTurn(int goal)
{
    float driveKp = 0.38;
    float driveKd = 0.0;

    int leftDriveError;
    int leftDriveLastError;
    int leftDriveDerivative;
    int rightDriveError;
    int rightDriveLastError;
    int rightDriveDerivative;
    int maxPower = 105;
    int allowableError = 0;

    leftDriveEnc = 0;
    rightDriveEnc = 0;
    clearTimer(T1);

    while(!(abs(leftDrive) > abs(goal) - allowableError) || !(abs(rightDrive) > abs(goal) - allowableError)){
    	if(userControl)
    		break;
        if(time1[T1] > abs(goal) * 5)
            break;

        // Proportional
        leftDriveError = (abs(goal) - abs(leftDrive));
        rightDriveError = (abs(goal) - abs(rightDrive));

        // Derivative
        leftDriveDerivative = leftDriveError - leftDriveLastError;
        rightDriveDerivative = rightDriveError - rightDriveLastError;
        leftDriveLastError = leftDriveError;
        rightDriveLastError = rightDriveError;

        // PID
        int leftPidDrive = round(((driveKp * leftDriveError) + (driveKd * leftDriveDerivative)));
        int rightPidDrive = round(((driveKp * rightDriveError) + (driveKd * rightDriveDerivative)));
        leftPidDrive = (abs(leftPidDrive) > maxPower) ? maxPower : leftPidDrive; // limit to a maxPower
        rightPidDrive = (abs(rightPidDrive) > maxPower) ? maxPower : rightPidDrive;

        rightPidDrive = (goal < 0) ? -rightPidDrive : rightPidDrive;
        leftPidDrive = (goal < 0) ? leftPidDrive : -leftPidDrive;

        drivePower(leftPidDrive, rightPidDrive);
    }

    if(goal > 0) {
  		drivePower(brakePower+5, -brakePower-5);
    } else {
        drivePower(-brakePower-5, brakePower+5);
    }
    wait1Msec(brakeTime);
    drivePower(0,0);
}

void drivePower(int left, int right)
{
	left = (abs(left) < threshold) ? 0 : left;
  	right = (abs(right) < threshold) ? 0 : right;

  	if((right != 0) || (left != 0)) {
  		clearTimer(T4);
  		releaseBrake();
  	}

  	motor[driveLeft] = left;
  	motor[driveRight] = right;
  	if(!punchersActivated)	{
  		motor[topLeftTrans] = left;
  		motor[botLeftTrans] = left;
  		motor[topRightTrans] = right;
  		motor[botRightTrans] = right;
  	}
}

void driveBrake(int direction) {
    if(direction < 0)
        drivePower(brakePower, brakePower);
    else
        drivePower(-brakePower, -brakePower);
    wait1Msec(brakeTime);
    drivePower(0, 0);
}

void gyroTurnTo(int goal, int direction)
{
	int difference = goal - SensorValue[gyroSensor];
	if(direction < 0)
		gyroTurn(-difference);
	else
		gyroTurn(difference);
}

void transPower(int left, int right)
{
	motor[topLeftTrans] = left;
  motor[botLeftTrans] = left;
  motor[topRightTrans] = right;
  motor[botRightTrans] = right;
}

void gyroTurn(int goal)
{
	gyro = 0;
	while(abs(gyro) < abs(goal))
    {
    	if(userControl)
    		break;
        int difference = abs(goal) - abs(gyro);
        int power = difference / 2;
        if(goal > 0) {
            drivePower(-power, power);
        } else {
            drivePower(power, -power);
        }
    }

    if(goal > 0) {
        drivePower(brakePower, -brakePower);
    } else {
  		drivePower(-brakePower, brakePower);
    }
    wait1Msec(brakeTime);
    drivePower(0,0);
  }
