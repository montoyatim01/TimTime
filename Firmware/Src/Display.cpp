#include "Display.h"
#include "Global.h"
#include "Menu.h"
#include "Calibration.h"
extern "C" {
#include "ssd1306.h"
}
#include "main.h"

//#include "fonts.h"
const char* foo = "0123456789";
char tcDisplay[11] = {0};
void updateDisplay(uint8_t state)
{
	switch (state)
	{
	case d_off:
		displayOff();
		break;
	case d_home:
		displayMain();
		break;
	case d_menu:
		displayMenu();
		break;
	case d_lock:
		displayLocked();
		break;
	case d_power:
		displayPower();
		break;
	case d_rateWarn:
		displayConfirmationRateChange();
		break;
	case d_offsetWarn:
		displayConfirmationOffsetChange();
		break;
	case d_rejamWarn:
		displayConfirmationRejam();
		break;
	case d_calibration:
		displayCalibration();
		break;
	case d_calOK:
		displayCalibrationOK();
		break;
	case d_calFail:
		displayCalibrationFail();
		break;
	case d_calReadFail:
		displayCalibrationReadFail();
		break;
	case d_userBits:
		displayUserBitsMenu();
		break;
	default:
		break;
	}
}

void displayOff()
{
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen(dispI2C);
}

void displayMain()
{
	//tcbreakout()
	char offsetStr[3];
	timecodeDisplay();
	ssd1306_SetCursor(4, 24);
	//TODO TC DISPLAY STRING
	ssd1306_WriteString(tcDisplay, Font_11x18, White);

	ssd1306_SetCursor(8, 0);
	switch (frameRate)
	{
	case 0:
		ssd1306_WriteString("23.98", Font_7x10, White);
		break;
	case 1:
		ssd1306_WriteString("24", Font_7x10, White);
		break;
	case 2:
		ssd1306_WriteString("25", Font_7x10, White);
		break;
	case 3:
		ssd1306_WriteString("29.97", Font_7x10, White);
		break;
	case 4:
		ssd1306_WriteString("29.97 DF", Font_7x10, White);
		break;
	case 5:
		ssd1306_WriteString("30", Font_7x10, White);
		break;
	}

	ssd1306_SetCursor(92, 52);
	ssd1306_WriteString("Menu", Font_7x10, White);

	
	if (tcJammed)
	{
		ssd1306_SetCursor(84, 0);
		ssd1306_WriteString("Jammed", Font_7x10, White);
	}
	else
	{
		ssd1306_SetCursor(90, 0);
		ssd1306_WriteString("Ready", Font_7x10, White);
	}
	if (intOffset != 30)
	{
		ssd1306_SetCursor(30, 12);
		if (intOffset > 30)
		{
			offsetStr[0] = '+';
			offsetStr[1] = foo[(intOffset-30)/10];
			offsetStr[2] = foo[(intOffset-30)%10];
		}
		else if (intOffset < 30)
		{
			offsetStr[0] = '-';		
			offsetStr[1] = foo[(30-intOffset)/10];
			offsetStr[2] = foo[(30-intOffset)%10];
		}
		ssd1306_WriteString(offsetStr, Font_7x10, White);
		ssd1306_SetCursor(48, 12);
		ssd1306_WriteString(" Offset", Font_7x10, White);
	}
	displayBattery();
	ssd1306_UpdateScreen(dispI2C);
}

void displayLocked()
{
	timecodeDisplay();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(44, 20);
	ssd1306_WriteString("Locked", Font_7x10, White);
	if (buttonsHeld)	
	{
		ssd1306_SetCursor(60, 32);
		char lockDisplay = foo[lockCountdown];
		ssd1306_WriteChar(lockDisplay, Font_7x10, White);	//TODO Lock Countdown
	}
	else
	{
		ssd1306_SetCursor(24, 30);
		ssd1306_WriteString("Hold < and >", Font_7x10, White);
		ssd1306_SetCursor(34, 40);
		ssd1306_WriteString("to unlock", Font_7x10, White);
	}

	ssd1306_SetCursor(2, 6);
	ssd1306_WriteString(tcDisplay, Font_7x10, White);

	ssd1306_SetCursor(90, 6);
	ssd1306_WriteString("23.98", Font_7x10, White);
	displayBattery();	
	ssd1306_UpdateScreen(dispI2C);
}

void displayPower()
{
	ssd1306_Fill(Black);
	ssd1306_SetCursor(14, 10);
	ssd1306_WriteString("Power Off", Font_11x18, White);
	ssd1306_SetCursor(60, 36);
	char powerDisplay = foo[powerCountdown];
	ssd1306_WriteChar(powerDisplay, Font_11x18, White); //TODO Power countdown
	ssd1306_UpdateScreen(dispI2C);
}

void displayMenu()
{
	ssd1306_Fill(Black);
	//Frame rate
	//Auto power off
	//Exit
	ssd1306_SetCursor(50, 0);
	ssd1306_WriteString("Menu", Font_7x10, White);
	ssd1306_SetCursor(10, 12);
	ssd1306_WriteString("Rate: ", Font_7x10, White);
	ssd1306_SetCursor(10, 22);
	ssd1306_WriteString("Offset: ", Font_7x10, White);
	ssd1306_SetCursor(10, 32);
	ssd1306_WriteString("Re-jam", Font_7x10, White);
	ssd1306_SetCursor(10, 42);
	ssd1306_WriteString("Auto-Off: ", Font_7x10, White);
	ssd1306_SetCursor(50, 52);
	ssd1306_WriteString("Exit", Font_7x10, White);

	ssd1306_SetCursor(50, 12);
	uint8_t dispRate;
	uint8_t dispOffset;
	char offsetStr[3];
	if (menuItemSelect){
		dispRate = rateAdjust;
		dispOffset = offsetAdjust;
	} else {
		dispRate = frameRate;
		dispOffset = intOffset;
	}
	switch (dispRate)
	{
	case 0:
		ssd1306_WriteString("23.98", Font_7x10, White);
		break;
	case 1:
		ssd1306_WriteString("24", Font_7x10, White);
		break;
	case 2:
		ssd1306_WriteString("25", Font_7x10, White);
		break;
	case 3:
		ssd1306_WriteString("29.97", Font_7x10, White);
		break;
	case 4:
		ssd1306_WriteString("29.97 DF", Font_7x10, White);
		break;
	case 5:
		ssd1306_WriteString("30", Font_7x10, White);
		break;
	}
	ssd1306_SetCursor(58, 22);
	//Insert offset
	if (dispOffset == 30)
	{
		offsetStr[0] = ' ';
		offsetStr[1] = '0';
		offsetStr[2] = '0';
	}
	else if (dispOffset > 30)
	{
		offsetStr[0] = '+';
		offsetStr[1] = foo[(dispOffset-30)/10];
		offsetStr[2] = foo[(dispOffset-30)%10];
	}
	else if (dispOffset < 30)
	{
		offsetStr[0] = '-';		
		offsetStr[1] = foo[(30-dispOffset)/10];
		offsetStr[2] = foo[(30-dispOffset)%10];
	}
	
	ssd1306_WriteString(offsetStr, Font_7x10, White);
	ssd1306_SetCursor(78,22);
	ssd1306_WriteString("     ", Font_7x10, White); 

	ssd1306_SetCursor(75, 42);
	switch (autoOff)
	{
	case 0:
		ssd1306_WriteString("1 hr", Font_7x10, White);
		break;
	case 1:
		ssd1306_WriteString("2 hr", Font_7x10, White);
		break;
	case 2:
		ssd1306_WriteString("4 hr", Font_7x10, White);
		break;
	case 3:
		ssd1306_WriteString("8 hr", Font_7x10, White);
		break;
	case 4:
		ssd1306_WriteString("12 hr", Font_7x10, White);
		break;
	}

	switch (menuItem)
	{
	case 0:
		if (menuItemSelect)
		{
			ssd1306_SetCursor(120, 12);
			ssd1306_WriteString("<", Font_7x10, White);
		}
		else
		{
			ssd1306_SetCursor(2, 12);
			ssd1306_WriteString(">", Font_7x10, White);
		}
		break;
	case 1:
		if (menuItemSelect)
		{
			ssd1306_SetCursor(120, 22);
			ssd1306_WriteString("<", Font_7x10, White);
		}
		else
		{
			ssd1306_SetCursor(2, 22);
			ssd1306_WriteString(">", Font_7x10, White);
		}

		break;
	case 2:
		if (menuItemSelect)
		{
			ssd1306_SetCursor(120, 32);
			ssd1306_WriteString("<", Font_7x10, White);
		}
		else
		{
			ssd1306_SetCursor(2, 32);
			ssd1306_WriteString(">", Font_7x10, White);
		}

		break;
	case 3:
		if (menuItemSelect)
		{
			ssd1306_SetCursor(120, 42);
			ssd1306_WriteString("<", Font_7x10, White);
		}
		else
		{
			ssd1306_SetCursor(2, 42);
			ssd1306_WriteString(">", Font_7x10, White);
		}

		break;
	case 4:
		ssd1306_SetCursor(41, 52);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(79, 52);
		ssd1306_WriteString("<", Font_7x10, White);
		break;
	}

	ssd1306_UpdateScreen(dispI2C);
}

void displayConfirmationRateChange()
{
	ssd1306_Fill(Black);
	ssd1306_SetCursor(8, 4);
	ssd1306_WriteString("Changing the rate", Font_7x10, White);
	ssd1306_SetCursor(16, 18);
	ssd1306_WriteString("will clear jam.", Font_7x10, White);
	ssd1306_SetCursor(32, 32);
	ssd1306_WriteString("Continue?", Font_7x10, White);
	ssd1306_SetCursor(30, 50);
	ssd1306_WriteString("NO", Font_7x10, White);
	ssd1306_SetCursor(80, 50);
	ssd1306_WriteString("YES", Font_7x10, White);
	if (highlightYes)
	{
		ssd1306_SetCursor(72, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(102, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	else
	{
		ssd1306_SetCursor(22, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(45, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	ssd1306_UpdateScreen(dispI2C);
}

void displayConfirmationOffsetChange()
{
	ssd1306_Fill(Black);
	ssd1306_SetCursor(10, 10);
	ssd1306_WriteString("The offset will", Font_7x10, White);
	ssd1306_SetCursor(14, 22);
	ssd1306_WriteString("take effect on", Font_7x10, White);
	ssd1306_SetCursor(36, 34);
	ssd1306_WriteString("next jam.", Font_7x10, White);
	//ssd1306_SetCursor(14, 34);
	//ssd1306_WriteString("Continue?", Font_7x10, White);
	//ssd1306_SetCursor(30, 48);
	//ssd1306_WriteString("NO", Font_11x18, White);
	ssd1306_SetCursor(52, 50);
	ssd1306_WriteString("OK", Font_7x10, White);
		ssd1306_SetCursor(44, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(66, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	
	ssd1306_UpdateScreen(dispI2C);
}

void displayConfirmationRejam()
{
	ssd1306_Fill(Black);
	ssd1306_SetCursor(12, 6);
	ssd1306_WriteString("This will clear", Font_7x10, White);
	ssd1306_SetCursor(24, 18);
	ssd1306_WriteString("current jam.", Font_7x10, White);
	ssd1306_SetCursor(34, 32);
	ssd1306_WriteString("Continue?", Font_7x10, White);
	ssd1306_SetCursor(30, 50);
	ssd1306_WriteString("NO", Font_7x10, White);
	ssd1306_SetCursor(80, 50);
	ssd1306_WriteString("YES", Font_7x10, White);
	if (highlightYes)
	{
		ssd1306_SetCursor(72, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(102, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	else
	{
		ssd1306_SetCursor(22, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(45, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	ssd1306_UpdateScreen(dispI2C);
}

void displayBattery()
{
	char battDisp[4];
	ssd1306_DrawBattery(batteryRemaining); //TODO BATTERY
	ssd1306_SetCursor(34, 52);
	int battPrint = int(batteryRemaining * 100.0);
	if (battPrint == 100)
	{
		battDisp[0] = '1';
		battDisp[1] = '0';
		battDisp[2] = '0';
		battDisp[3] = '%';
	}
	else
	{
		battDisp[0] = ' ';
		battDisp[1] = foo[battPrint / 10];
		battDisp[2] = foo[battPrint % 10];
		if (!stat1 && stat2)
			battDisp[3] = '^';
		else if (stat1 && !stat2)
			battDisp[3] = '!';
		else
			battDisp[3] = '%';
	}
	battDisp[4] = ' ';
	ssd1306_WriteString(battDisp, Font_7x10, White);
}
void timecodeDisplay()
{
	tcDisplay[0] = foo[hr / 10];
	tcDisplay[1] = foo[hr % 10];
	tcDisplay[2] = ':';
	tcDisplay[3] = foo[mn / 10];
	tcDisplay[4] = foo[mn % 10];
	tcDisplay[5] = ':';
	tcDisplay[6] = foo[sc / 10];
	tcDisplay[7] = foo[sc % 10];
	tcDisplay[8] = (frameRate == 4) ? ';' : ':';
	tcDisplay[9] = foo[fr / 10];
	tcDisplay[10] = foo[fr % 10];
}
void initDisplay()
{
	 if (ssd1306_Init(dispI2C) != 0) {
     Error_Handler();
   }
}

void displayCalibration(){
	char calStr[8];
	calStr[0] = foo[(newCal % 100000000)/10000000];
	calStr[1] = foo[(newCal % 10000000)/1000000];
	calStr[2] = foo[(newCal % 1000000)/100000];
	calStr[3] = foo[(newCal % 100000)/10000];
	calStr[4] = foo[(newCal % 10000)/1000];
	calStr[5] = foo[(newCal % 1000)/100];
	calStr[6] = foo[(newCal % 100)/10];
	calStr[7] = foo[newCal % 10];
	ssd1306_Fill(Black);
	ssd1306_SetCursor(24, 0);
	ssd1306_WriteString("Calibration", Font_7x10, White);
	ssd1306_SetCursor(10, 12);
	ssd1306_WriteString("Val:", Font_7x10, White);
	ssd1306_SetCursor(40, 12);
	ssd1306_WriteString(calStr, Font_7x10, White);
	ssd1306_SetCursor(96,12);
	ssd1306_WriteString("    ",Font_7x10,White);
	ssd1306_SetCursor(14, 50);
	ssd1306_WriteString("OK", Font_7x10, White);
	ssd1306_SetCursor(60, 50);
	ssd1306_WriteString("Cancel", Font_7x10, White);
	if (menuItem == 0){
		if (menuItemSelect){
			ssd1306_SetCursor(120, 12);
			ssd1306_WriteString("<", Font_7x10, White);
		} else {
			ssd1306_SetCursor(2, 12);
			ssd1306_WriteString(">", Font_7x10, White);
		}
	}
	if (menuItem == 1)
	{
		ssd1306_SetCursor(6, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(28, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	if (menuItem == 2)
	{
		ssd1306_SetCursor(52, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(102, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	ssd1306_UpdateScreen(dispI2C);


}
void displayCalibrationOK(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(24, 0);
	ssd1306_WriteString("Calibration", Font_7x10, White);
	ssd1306_SetCursor(38,12);
	ssd1306_WriteString("Success", Font_7x10, White);

	ssd1306_SetCursor(58,50);
	ssd1306_WriteString("OK", Font_7x10, White);
	if (menuItemSelect){
		ssd1306_SetCursor(50, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(72, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	ssd1306_UpdateScreen(dispI2C);

}
void displayCalibrationFail(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(24, 0);
	ssd1306_WriteString("Calibration", Font_7x10, White);
	ssd1306_SetCursor(50,12);
	ssd1306_WriteString("Fail", Font_7x10, White);
	ssd1306_SetCursor(58,50);
	ssd1306_WriteString("OK", Font_7x10, White);
	if (menuItemSelect){
		ssd1306_SetCursor(50, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(72, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	ssd1306_UpdateScreen(dispI2C);


}
void displayCalibrationReadFail(){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("Failed to read", Font_7x10, White);
	ssd1306_SetCursor(0,12);
	ssd1306_WriteString("calibration value.", Font_7x10, White);
	ssd1306_SetCursor(0,24);
	ssd1306_WriteString("Device will drift", Font_7x10, White);
	ssd1306_SetCursor(0,36);
	ssd1306_WriteString("quickly", Font_7x10, White);
	ssd1306_SetCursor(58,50);
	ssd1306_WriteString("OK", Font_7x10, White);
	if (menuItemSelect){
		ssd1306_SetCursor(50, 50);
		ssd1306_WriteString(">", Font_7x10, White);
		ssd1306_SetCursor(72, 50);
		ssd1306_WriteString("<", Font_7x10, White);
	}
	ssd1306_UpdateScreen(dispI2C);
}

void displayUserBitsMenu(){

	ssd1306_Fill(Black);
	ssd1306_SetCursor(32, 0);
	ssd1306_WriteString("User Bits", Font_7x10, White);
	
	uint8_t startX = 20;
	ssd1306_SetCursor((16+(8*0)),28);
	ssd1306_WriteChar(foo[displayUserBits[0]], Font_7x10, White);
	ssd1306_SetCursor((16+(8*1)),28);
	ssd1306_WriteChar(foo[displayUserBits[1]], Font_7x10, White);
	ssd1306_SetCursor((16+(8*2)),28);
	ssd1306_WriteChar(':', Font_7x10, White);

	ssd1306_SetCursor((16+(8*3)),28);
	ssd1306_WriteChar(foo[displayUserBits[2]], Font_7x10, White);
	ssd1306_SetCursor((16+(8*4)),28);
	ssd1306_WriteChar(foo[displayUserBits[3]], Font_7x10, White);
	ssd1306_SetCursor((16+(8*5)),28);
	ssd1306_WriteChar(':', Font_7x10, White);


	ssd1306_SetCursor((16+(8*6)),28);
	ssd1306_WriteChar(foo[displayUserBits[4]], Font_7x10, White);
	ssd1306_SetCursor((16+(8*7)),28);
	ssd1306_WriteChar(foo[displayUserBits[5]], Font_7x10, White);
	ssd1306_SetCursor((16+(8*8)),28);
	ssd1306_WriteChar(':', Font_7x10, White);


	ssd1306_SetCursor((16+(8*9)),28);
	ssd1306_WriteChar(foo[displayUserBits[6]], Font_7x10, White);
	ssd1306_SetCursor((16+(8*10)),28);
	ssd1306_WriteChar(foo[displayUserBits[7]], Font_7x10, White);

	ssd1306_SetCursor(20,50);
	ssd1306_WriteString("Set", Font_7x10, White);

	ssd1306_SetCursor(64, 50);
	ssd1306_WriteString("Cancel", Font_7x10, White);

	if (!menuItemSelect){
	
	if (menuItem == 0){
		ssd1306_SetCursor(8,28);
		ssd1306_WriteChar('>', Font_7x10, White);
		ssd1306_SetCursor(106, 28);
		ssd1306_WriteChar('<', Font_7x10, White);
	}
	if (menuItem == 1){
		ssd1306_SetCursor(12, 50);
		ssd1306_WriteChar('>', Font_7x10, White);
		ssd1306_SetCursor(40, 50);
		ssd1306_WriteChar('<', Font_7x10, White);
	}
	if (menuItem == 2){
		ssd1306_SetCursor(56, 50);
		ssd1306_WriteChar('>', Font_7x10, White);
		ssd1306_SetCursor(106, 50);
		ssd1306_WriteChar('<', Font_7x10, White);
	}
	} else {
		switch (menuItem){
			case 0:
				ssd1306_SetCursor((16+(8*0)),40);
				ssd1306_WriteChar('^', Font_7x10, White);
				break;
			case 1:
				ssd1306_SetCursor((16+(8*1)),40);
				ssd1306_WriteChar('^', Font_7x10, White);
				break;
			case 2:
				ssd1306_SetCursor((16+(8*3)),40);
				ssd1306_WriteChar('^', Font_7x10, White);
				break;
			case 3:
				ssd1306_SetCursor((16+(8*4)),40);
				ssd1306_WriteChar('^', Font_7x10, White);
				break;
			case 4:
				ssd1306_SetCursor((16+(8*6)),40);
				ssd1306_WriteChar('^', Font_7x10, White);
				break;
			case 5:
				ssd1306_SetCursor((16+(8*7)),40);
				ssd1306_WriteChar('^', Font_7x10, White);
				break;
			case 6:
				ssd1306_SetCursor((16+(8*9)),40);
				ssd1306_WriteChar('^', Font_7x10, White);
				break;
			case 7:
				ssd1306_SetCursor((16+(8*10)),40);
				ssd1306_WriteChar('^', Font_7x10, White);
				break;
			case 8:
				ssd1306_SetCursor(8,28);
				ssd1306_WriteChar('>', Font_7x10, White);
				break;
		}
		if (highlightYes){
			switch (menuItem){
			case 0:
				ssd1306_SetCursor((16+(8*0)),12);
				ssd1306_WriteChar('v', Font_7x10, White);
				break;
			case 1:
				ssd1306_SetCursor((16+(8*1)),12);
				ssd1306_WriteChar('v', Font_7x10, White);
				break;
			case 2:
				ssd1306_SetCursor((16+(8*3)),12);
				ssd1306_WriteChar('v', Font_7x10, White);
				break;
			case 3:
				ssd1306_SetCursor((16+(8*4)),12);
				ssd1306_WriteChar('v', Font_7x10, White);
				break;
			case 4:
				ssd1306_SetCursor((16+(8*6)),12);
				ssd1306_WriteChar('v', Font_7x10, White);
				break;
			case 5:
				ssd1306_SetCursor((16+(8*7)),12);
				ssd1306_WriteChar('v', Font_7x10, White);
				break;
			case 6:
				ssd1306_SetCursor((16+(8*9)),12);
				ssd1306_WriteChar('v', Font_7x10, White);
				break;
			case 7:
				ssd1306_SetCursor((16+(8*10)),12);
				ssd1306_WriteChar('v', Font_7x10, White);
				break;
		}
		}
	}


	ssd1306_UpdateScreen(dispI2C);

}

