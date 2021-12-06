#include "Display.h"

void updateDisplay(displayMode state){
    switch(state){
        case off:
            break;
        case main:
            break;
        case menu:
            break;
        case lock:
            break;
        case power:
            break;
        case 0x5:
            break;
        default:
            break;
    }

}

void displayOff(){
    ssd1306_Fill(Black);
	ssd1306_UpdateScreen(&hi2c2);
}

void displayMain(){
//tcbreakout()
//tcDisplay
    ssd1306_SetCursor(4,24);
	ssd1306_WriteString(tcDisplay,Font_11x18,White);

	ssd1306_SetCursor(4,6);
	ssd1306_WriteString("23.98",Font_7x10,White);

	ssd1306_SetCursor(92,52);
	ssd1306_WriteString("Menu",Font_7x10,White);

    displayBattery();
    ssd1306_UpdateScreen(&hi2c2);
}

void displayLocked(){
    timecodeDisplay();
    ssd1306_Fill(Black);
		ssd1306_SetCursor(44,20);
		ssd1306_WriteString("Locked", Font_7x10, White);
        if (buttonsHeld){
            ssd1306_SetCursor(60,32);
		//lockDisplay = foo[lockCountdown];
		ssd1306_WriteChar(lockDisplay, Font_7x10, White);
        } else {
            ssd1306_SetCursor(24,30);
		ssd1306_WriteString("Hold < and >", Font_7x10, White);
        ssd1306_SetCursor(34,40);
		ssd1306_WriteString("to unlock", Font_7x10, White);
        }
		
		ssd1306_SetCursor(2,6);
		      ssd1306_WriteString(tcDisplay,Font_7x10,White);

		      ssd1306_SetCursor(90,6);
		      ssd1306_WriteString("23.98",Font_7x10,White);
              displayBattery();
              ssd1306_UpdateScreen(&hi2c2);
}

void displayPower(){
ssd1306_Fill(Black);
		ssd1306_SetCursor(14,10);
		ssd1306_WriteString("Power Off", Font_11x18, White);
		ssd1306_SetCursor(60,36);
		//powerDisplay = foo[powerCountdown];
		ssd1306_WriteChar(powerDisplay, Font_11x18, White);
		ssd1306_UpdateScreen(&hi2c2);
}

void displayMenu(){
ssd1306_Fill(Black);
		//Frame rate
		//Auto power off
		//Exit
		ssd1306_SetCursor(50,2);
		ssd1306_WriteString("Menu", Font_7x10, White);
		ssd1306_SetCursor(10,14);
		ssd1306_WriteString("Rate: ",Font_7x10, White);
		ssd1306_SetCursor(10,24);
		ssd1306_WriteString("Offset: ",Font_7x10, White);
        ssd1306_SetCursor(10,34);
		ssd1306_WriteString("Re-jam",Font_7x10, White);
        ssd1306_SetCursor(10,44);
		ssd1306_WriteString("Auto-Off: ",Font_7x10, White);
		ssd1306_SetCursor(50,54);
		ssd1306_WriteString("Exit",Font_7x10, White);

		ssd1306_SetCursor(50,14);
		switch (frameRate){
		case 0:
			ssd1306_WriteString("23.98",Font_7x10, White);
			break;
		case 1:
			ssd1306_WriteString("24",Font_7x10, White);
			break;
		case 2:
			ssd1306_WriteString("25",Font_7x10, White);
			break;
		case 3:
			ssd1306_WriteString("29.97",Font_7x10, White);
			break;
		case 4:
			ssd1306_WriteString("29.97 DF",Font_7x10, White);
			break;
		case 5:
			ssd1306_WriteString("30",Font_7x10, White);
			break;
		}
		ssd1306_SetCursor(75,24);
		switch (autoOff){
		case 0:
			ssd1306_WriteString("1 hr",Font_7x10, White);
			break;
		case 1:
			ssd1306_WriteString("2 hr",Font_7x10, White);
			break;
		case 2:
			ssd1306_WriteString("4 hr",Font_7x10, White);
			break;
		case 3:
			ssd1306_WriteString("8 hr",Font_7x10, White);
			break;
		case 4:
			ssd1306_WriteString("12 hr",Font_7x10, White);
			break;
		}

			switch (menuItem){
			case 0:
				if (menuItemSelect){
					ssd1306_SetCursor(120,14);
					ssd1306_WriteString("<",Font_7x10, White);
				}else {
					ssd1306_SetCursor(2,14);
					ssd1306_WriteString(">",Font_7x10, White);
				}
				break;
			case 1:
				if (menuItemSelect){
					ssd1306_SetCursor(120,24);
					ssd1306_WriteString("<",Font_7x10, White);
				} else {
					ssd1306_SetCursor(2,24);
					ssd1306_WriteString(">",Font_7x10, White);
				}

				break;
			case 2:
				ssd1306_SetCursor(40,50);
				ssd1306_WriteString(">",Font_7x10, White);
				ssd1306_SetCursor(79,50);
				ssd1306_WriteString("<",Font_7x10, White);
				break;
			}

		ssd1306_UpdateScreen(&hi2c2);
}

void displayConfirmationRateChange(){
    ssd1306_Fill(Black);
		ssd1306_SetCursor(14,10);
		ssd1306_WriteString("Changing the frame rate", Font_7x10, White);
        ssd1306_SetCursor(14,22);
		ssd1306_WriteString("will clear the current jam", Font_7x10, White);
        ssd1306_SetCursor(14,34);
		ssd1306_WriteString("Continue?", Font_7x10, White);
		ssd1306_SetCursor(30,48);
		ssd1306_WriteString("NO", Font_11x18, White);
        ssd1306_SetCursor(80,48);
		ssd1306_WriteString("YES", Font_11x18, White);
        if (highlightYes){
            ssd1306_SetCursor(20,48);
		ssd1306_WriteString(">", Font_11x18, White);
        ssd1306_SetCursor(70,48);
		ssd1306_WriteString("<", Font_11x18, White);
        } else {
            ssd1306_SetCursor(72,48);
		ssd1306_WriteString(">", Font_11x18, White);
        ssd1306_SetCursor(100,48);
		ssd1306_WriteString("<", Font_11x18, White);
        }
		ssd1306_UpdateScreen(&hi2c2);

}

void displayConfirmationRejam(){
    ssd1306_Fill(Black);
		ssd1306_SetCursor(14,10);
		ssd1306_WriteString("Are you sure you want", Font_7x10, White);
        ssd1306_SetCursor(14,22);
		ssd1306_WriteString("to clear the current jam", Font_7x10, White);
        ssd1306_SetCursor(14,34);
		ssd1306_WriteString("and re-jam?", Font_7x10, White);
		ssd1306_SetCursor(30,48);
		ssd1306_WriteString("NO", Font_11x18, White);
        ssd1306_SetCursor(80,48);
		ssd1306_WriteString("YES", Font_11x18, White);
        if (highlightYes){
            ssd1306_SetCursor(20,48);
		ssd1306_WriteString(">", Font_11x18, White);
        ssd1306_SetCursor(70,48);
		ssd1306_WriteString("<", Font_11x18, White);
        } else {
            ssd1306_SetCursor(72,48);
		ssd1306_WriteString(">", Font_11x18, White);
        ssd1306_SetCursor(100,48);
		ssd1306_WriteString("<", Font_11x18, White);
        }
		ssd1306_UpdateScreen(&hi2c2);

}

void displayBattery(){
ssd1306_DrawBattery(battStatus);
		      ssd1306_SetCursor(34,52);
		      battPrint = int(battStatus * 100.0);
		      if (battPrint == 100){
		    	  battDisp[0] = '1';
		    	  battDisp[1] = '0';
		    	  battDisp[2] = '0';
		    	  battDisp[3] = '%';
		      }else {
		    	  battDisp[0] = ' ';
		    	  battDisp[1] = foo[battPrint / 10];
		    	  battDisp[2] = foo[battPrint % 10];
		    	  if (!stat1 && stat2) battDisp[3] = '^';
		    	  else if (stat1 && !stat2) battDisp[3] = '!';
		    	  else battDisp[3] = '%';
		      }
		      battDisp[4] = ' ';
		      ssd1306_WriteString(battDisp,Font_7x10,White);
}
void timecodeDisplay(){
    tcDisplay[0] = foo[hr / 10];
	tcDisplay[1] = foo[hr % 10];
	tcDisplay[2] = ':';
	tcDisplay[3] = foo[mn / 10];
	tcDisplay[4] = foo[mn % 10];
	tcDisplay[5] = ':';
	tcDisplay[6] = foo[sc / 10];
	tcDisplay[7] = foo[sc % 10];
	tcDisplay[8] = (rate == 4) ? ';' : ':';
	tcDisplay[9] = foo[fr / 10];
	tcDisplay[10] = foo[fr % 10];
}

		 


	case 0x05:	//Menu
		

		break;