#include "EV3Servo-lib-UW.c"
const int MAJORARM = 1;
const int UP_AND_DOWN = 2;
const int ROTATE_SCOOP = 3;
const int BOWL = 0;
const int VANILLA = -90;
const int REST = -180;
const int CHOCOLATE = -270;
const int NEWICEADJUST = 5;

void checkAngle (int destination);
bool getMenuOption(int & iCOption, bool & sOption);
void shakeSprinkles();
void rotateMain(int destination);
void scoop (bool & f1_scooped, bool & f2_scooped, int & iCOption);
int bowlStatus (bool scooped, bool dropped);
void bowlReturn(int status);
void shake (bool & dropped);

task main()
{
	SensorType[S1] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
	SensorType[S2] = sensorEV3_Touch;
	SensorMode[S3] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_Color;
	wait1Msec(50);
	SensorType[S4] = sensorI2CCustom9V;
	setServoPosition(S4, UP_AND_DOWN, -90);
	setServoPosition(S4, ROTATE_SCOOP, 90);
	setServoSpeed(S4, MAJORARM, 0);

	int iCOption = 0, status = 0;
	bool sOption = false, end = false, f1_scooped = false, f2_scooped = false;
	bool dropped = false, scooped = false;

  end = getMenuOption(iCOption, sOption);
	while (!end) {
		resetGyro(S1);
		scooped = false;
		dropped = false;

		rotateMain(iCOption);
		wait1Msec(150);
		if (abs(SensorValue[S1] - iCOption) > 8) {
			checkAngle(iCOption);
		}

		status = bowlStatus(scooped, dropped);
		bowlReturn(status);

		status = 0;

		scoop(f1_scooped, f2_scooped, iCOption);
		scooped = true;

		status = bowlStatus(scooped, dropped);
		bowlReturn(status);
		status = 0;

		rotateMain(BOWL);
		wait1Msec(150);
		if (abs(SensorValue[S1] - BOWL) > 8) {
			checkAngle(BOWL);
		}

		status = bowlStatus(scooped, dropped);
		bowlReturn(status);
		status = 0;

		setServoPosition(S4, ROTATE_SCOOP, -90);
		wait1Msec(500);
		shake(dropped);
		wait1Msec(500);

		setServoPosition(S4, ROTATE_SCOOP, 90);

		rotateMain(REST);

		if (sOption) {
			status = bowlStatus(scooped, dropped);
			bowlReturn(status);
			status = 0;
			shakeSprinkles();
		}

		displayBigTextLine(2, "Order ready!");
		displayBigTextLine(4, "Remove bowl!");

		while (SensorValue[S3] != 0) {
		}

		eraseDisplay();

		wait1Msec(1000);
		displayBigTextLine(1, "Place bowl.");

		while(SensorValue[S3] == 0) {
		}

		eraseDisplay();
		setServoSpeed(S4, MAJORARM, 18);
		wait1Msec(2000);
		setServoSpeed(S4, MAJORARM, 0);
		end = getMenuOption(iCOption, sOption);
	}
}

bool getMenuOption (int & iCOption, bool & sOption) {
	bool reset = false;
	do {
		reset = false;
		string iceCream = "", sprinkles = "";
		displayBigTextLine(1, "Press enter");
		displayBigTextLine(3, "to begin");
		displayBigTextLine(5, "your order!");

		while (!getButtonPress(buttonEnter)) {
		}

		if (getButtonPress(buttonEnter)) {
			time1[T1] = 0;
		}

		while (getButtonPress(buttonEnter)) {
		}

		eraseDisplay();

		if (time1[T1] > 10000) {
			displayBigTextLine(1, "Shutting Down.");
			return true;
		}

		displayBigTextLine(2, "Press up");
		displayBigTextLine(4, "for vanilla.");
		displayBigTextLine(6, "Press down");
		displayBigTextLine(8, "for chocolate.");

		while (!getButtonPress(buttonUp) && !getButtonPress(buttonDown)) {
		}

		if (getButtonPress(buttonUp)) {
			iCOption = VANILLA;
			iceCream = "Vanilla";
			displayBigTextLine(10, "Vanilla");
		} else {
			iCOption = CHOCOLATE;
			iceCream = "Chocolate";
			displayBigTextLine(10, "Chocolate");
		}

		eraseDisplay();

		displayBigTextLine(2, "Press right");
		displayBigTextLine(4, "for sprinkles.");
		displayBigTextLine(6, "Press left");
		displayBigTextLine(8, "for none.");

		while (!getButtonPress(buttonLeft) && !getButtonPress(buttonRight)) {
		}

		if (getButtonPress(buttonLeft)) {
			sOption = false;
			sprinkles = "No";
			displayBigTextLine(10, "No");
		} else {
			sOption = true;
			sprinkles = "Yes";
			displayBigTextLine(10, "Yes");
		}

		wait1Msec(1000);

		eraseDisplay();
		displayBigTextLine(1, "Ice Cream:", iceCream);
		displayBigTextLine(3, "%s", iceCream);
		displayBigTextLine(5, "Sprinkles:", sprinkles);
		displayBigTextLine(7, "%s", sprinkles);
		displayBigTextLine(9, "Press enter");
		displayBigTextLine(11, "to confirm.");


		time1[T1] = 0;

		while (!getButtonPress(buttonEnter) &&  time1[T1] < 30000) {
			displayBigTextLine(14, "Time: %d", (30 - time1[T1]/1000));
		}

		if (time1[T1] > 30000) {
			reset = true;
		}
		eraseDisplay();
	} while (reset);
	return false;
}

void shakeSprinkles () {
	nMotorEncoder[motorB] = 0;
	for (int count = 1; count < 3; count ++) {
		motor[motorB] = 10;
		while (abs(nMotorEncoder[motorB]) < 180)
		{}
		motor[motorB] = -10;
		while (abs(nMotorEncoder[motorB]) > 0)
		{}
	}
	motor[motorB] = 0;
}


void rotateMain (int destination)
{
	int startAngle = SensorValue[S1];
	if (startAngle != destination)
	{
			if(startAngle > destination){
				setServoSpeed(S4, MAJORARM, -18);
				while (SensorValue[S1] > destination) {
				}
		}
		else {
				setServoSpeed(S4, MAJORARM, 18);
			while (SensorValue[S1] < destination) {
			}
		}
	}
	setServoSpeed(S4, MAJORARM, 0);


}

void scoop (bool & f1_scooped, bool & f2_scooped, int & iCOption)
{
	if (iCOption == VANILLA && f1_scooped) {
		rotateMain(SensorValue[S1] - NEWICEADJUST);
		f1_scooped = false;
	} else if (iCOption == CHOCOLATE && f2_scooped) {
		rotateMain(SensorValue[S1] - NEWICEADJUST);
		f2_scooped = false;
	} else if (VANILLA && !f1_scooped) {
		f1_scooped = true;
	} else if (CHOCOLATE && !f2_scooped) {
		f2_scooped = true;
	}

	setServoPosition(S4, ROTATE_SCOOP, -90);
	wait1Msec(1000);
	setServoPosition(S4, UP_AND_DOWN, 15);
	wait1Msec(1000);
	setServoPosition(S4, ROTATE_SCOOP, 90);
	wait1Msec(1000);
	setServoPosition(S4, UP_AND_DOWN, -90);
	wait1Msec(1000);
}


int bowlStatus(bool scooped, bool dropped) {
    if (SensorValue[S3] == 0){ //colour sensor
        if (scooped)
            if (dropped)
                return 3;
            else
                return 2;
        else
            return 1;
    }
    return 0;
}

void bowlReturn(int status) {
    if (status == 1) {
        eraseDisplay();

        while (SensorValue[S3] == 0) {
            displayBigTextLine(2,"Put the ");
            displayBigTextLine(4, "bowl back!!!!");
       	}

         eraseDisplay();
    }

    else if (status == 2) {
        eraseDisplay();
        if (SensorValue[S1] < REST) {
					rotateMain(REST);
				}
        rotateMain(REST);//to rest position


        while (SensorValue[S3] == 0) {
            displayBigTextLine(2,"Put the ");
            displayBigTextLine(4, "bowl back!!!!");
       	}

        eraseDisplay();
        if (SensorValue[S1] < BOWL) {
					rotateMain(BOWL);
				}
        rotateMain(BOWL);//to bowl position
    }

    else if (status == 3) {
        eraseDisplay();
        if (SensorValue[S1] < REST) {
					rotateMain(REST);
				}
        rotateMain(REST);//to rest position
        wait1Msec(1000);
        while (SensorValue[S3] == 0) {
            displayBigTextLine(2,"Put the ");
            displayBigTextLine(4, "bowl back!!!!");
       	}

        eraseDisplay();
    }
}

void checkAngle (int destination) {
	while (abs(SensorValue[S1] - destination) > 8) {
		rotateMain(destination);
		wait1Msec(150);
	}
	wait1Msec(400);
	eraseDisplay();
}

void shake (bool & dropped) {
	while (SensorValue[S2] == 0) {
		setServoPosition(S4, UP_AND_DOWN, -40);
		wait1Msec(500);
		setServoPosition(S4, UP_AND_DOWN, -90);
		wait1Msec(500);
	}
	dropped = true;
}
