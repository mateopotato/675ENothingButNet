#define lock SensorValue[lockPistons]
#define deployer SensorValue[deployerPiston]
#define brake SensorValue[brakePistons]

#define rampBtn vexRT[Btn7D]
#define rampBtn2 vexRT[Btn7L]
#define brakeBtn vexRT[Btn5U]

bool braking = false;
bool cubicMapping = false;
bool lastRampBtn = false;
bool autoBrake = true;
int threshold = 12;
int liftCount = 0;
int stillTime = 0;
int autoBrakeTime = 3000;
int breakPower = 10;

int mapped(int x);
task drive();
task pneumatics();
void drivePower(int left, int right);
void turn(int degrees);
void releaseBrake();
void actuateBrake();
void releaseLift();
void lockLift();
void deploy();

task drive()
{
	int lastTime = nSysTime;
	while(true) {
		stillTime += nSysTime - lastTime;
		lastTime = nSysTime;
		drivePower(vexRT(Ch3), vexRT(Ch2));
	}
}

task pneumatics()
{
	bool lastBrakeBtn = false;
	deployer = 0;
	lockLift();
	actuateBrake();
	while(true)
	{
		/* Ramp */
		if(rampBtn == 1 && rampBtn2 == 1 && lastRampBtn == false) {
			if(liftCount == 0) {
				deploy();
				liftCount++;
			} else if(liftCount == 1) {
				releaseLift();
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
		if((abs(vexRT[Ch2]) > threshold) || (abs(vexRT[Ch3]) > threshold))
			releaseBrake();
		if((stillTime > autoBrakeTime) && (autoBrake == true))
			actuateBrake();
	}
}

void deploy()
{
	deployer = 1;
	wait1Msec(500);
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
	brake = 1;
	braking = true;
}

void releaseBrake()
{
	brake = 0;
	braking = false;
}


void gyroTurn(int goal){
    float gyroKp = 0.10;
    float gyroKd = 0.5;
    
    int gyroError;
    int gyroLastError
    int gyroDerivative
    int maxPower = 105;
    int allowableError = 20;
    
    while(!(gyro < goal + allowableError) && !(gyro > goal - allowableError)){
        
        // Proportional
        gyroError = (goal - gyro);
        
        // Derivative
        gyroDerivative = gyroError - gyroLastError;
        GyroLast_error = GyroError;
        
        // PID
        int pidDrive = round(((gyroKp * gyroError) + (gyroKd * gyroDerivative)));
        pidDrive = (abs(pidDrive) > maxPower) ? maxPower : pidDrive; // limit to a maxPower
        
        drivePower(-pidDrive, pidDrive)
    }
    int brake = (pidDrive > 0) ? brakePower : -brakePower;
    drivePower(brake, -brake);
    wait1Msec(brakeTime);
    drivePower(0, 0);
    
}

void driveDistance(int goal)
{
    float driveKp = 0.10;
    float driveKd = 0.5;
    
    int leftDriveError;
    int leftDriveLastError;
    int leftDriveDerivative;
    int rightDriveError;
    int rightDriveLastError;
    int rightDriveLastError;
    int maxPower = 105;
    int allowableError = 20;
    
    leftDrive = 0;
    rightDrive = 0;
    
    while((!(leftDrive < goal + allowableError) && !(leftDrive > goal - allowableError)) && (!(rightDrive < goal + allowableError) && !(rightDrive > goal - allowableError))){
        
        // Proportional
        leftDriveError = (goal - leftDrive);
        rightDriveError = (goal - rightDrive);
        
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
        
        drivePower(leftPidDrive, rightPidDrive)
    }
    driveBrake();
}


void drivePower(int left, int right)
{

	if(abs(left) < threshold)
		left = 0;
	if(abs(right) < threshold)
		right = 0;
	if((abs(right) >= threshold) || (abs(left) >= threshold))
		stillTime = 0;
    if (cubicMapping) {
        left = mapped(left);
        right = mapped(right);
    }
	motor[rightback] = right;
	motor[rightfront] = right;
	motor[leftback] = left;
	motor[leftfront] = left;
}

void driveBrake() {
    drivePower(brakePower, brakePower);
    wait1Msec(brakeTime);
    drivePower(0, 0);
}

int mapped(int x)
{
    return round(0.0001*x*x*x - 0.0095*x*x + 0.4605*x - 0.6284);
}
