#include "Timecode.h"
#include "main.h"
/* Timecode based functions */

/* Init Timecode
* Get the current frame count based on the data read in from the timer
*TODO: Add in the user variables
*/
bool initTimecode(){
  int hr = int(tcIN[1] & 0x0F) + (int(tcIN[0] & 0x03)*10);
  int mn = int(tcIN[3] & 0x0F) + (int(tcIN[2] & 0x07)*10);
  int sc = int(tcIN[5] & 0x0F) + (int(tcIN[4] & 0x07)*10);
  int fr = int(tcIN[7] & 0x0F) + (int(tcIN[6] & 0x03)*10);
  clockFrame = (hr * 60 * 60 * frameRateDivisor[frameRate]) + (mn * 60 * frameRateDivisor[frameRate]) + (sc * frameRateDivisor[frameRate]) + fr;
  HAL_TIM_Base_Start_IT(countTIM);
  HAL_TIM_Base_Start_IT(outTIM);
}

/* Setting stricter limits on the timing of the input signal:
* Tougher constraints mean more accurate reporting of the
* frame rate of the incoming signal, at the expensve of
* failing to jam to devices not strictly in-line/in-spec
*
* Jam confirmation is an opportunity for more devices
* that may be out of spec to still jam, while verifying
* that the frame rates are identical between the two devices
*/

/* Jam Confirmation
* Begin running off of the read timecode at the set rate.
* Wait a period of time (25 seconds)
*
*/
bool jamConfirmation(){

}


/*Timecode Breakout
* Input the rate, current frame count, and pointers to the variables
* to break the frame count down by hour, minute, second, and frame
*/
bool timecodeBreakout(uint8_t* hour, uint8_t* min, uint8_t* sec, uint8_t* frame, uint32_t currentFrame, uint8_t rate){
  *hour = (currentFrame / rate) / 3600;
  *min = ((currentFrame / rate) / 60 ) % 60;
  *sec = (currentFrame / rate) % 60;
  *frame = currentFrame % rate;
}

/* Reset Timecode
* Simple function to reset all timers and variables back to
* pre-jam state
*/
bool resetTimecode(){
    clockFrame = 0;
    HAL_TIM_Base_Stop_IT(countTIM);
    HAL_TIM_Base_Stop_IT(outTIM);
    HAL_TIM_IC_Start_IT(inTIM, TIM_CHANNEL_3);
    //timer2 input yes
    //timer6 output no
}

/*TODO Write timecode function?
*
*/


/* Calibration
* This function runs on startup and determines the necesarry compensation
* values for the timers based on the measured frequency of the TCXO.
*
* Function takes the measured frequency of the TCXO, and divides 
* it by the prescaler (in this case, 20) to get the frequency of the timer.
* This is then multiplied by the ticks per frame, based on the frame rate.
* IE 23.98 is 1001/24000, so divided by 1001.
* 
* The compensation value is calculated by subtracting this value from 
* the frame rate period multipled by the frame rate divisor
* IE 23.98 is 1001/24000, so 50049 * 24000
* 
* For higher accuracy, the compensation runs at a factor of 10 times
* the frame rate divisor.
* IE 23.98 is 1001/24000, so it runs every 240,000 frames.
*/
void calibrate(){
  int32_t calibrationArray[6];
	calibrationArray[0] = 1201200000 - (int32_t(	(double(calibration)/20.0) * 1001));
	calibrationArray[1] = 1200000000 - (int32_t(	(double(calibration)/20.0) * 1000));
	calibrationArray[2] = 1200000000 - (int32_t(	(double(calibration)/20.0) * 1000));
	calibrationArray[3] = 1201200000 - (int32_t(	(double(calibration)/20.0) * 1001));
  calibrationArray[4] = 1201200000 - (int32_t(	(double(calibration)/20.0) * 1001));
	calibrationArray[5] = 1200000000 - (int32_t(	(double(calibration)/20.0) * 1000));
}

/* Frame Check
*
*
*
*/

void frameCheck()
{
  switch(frameRate)
  {
    case 0: //23.98
      if (clockFrame == 2073600)
        clockFrame = 0;
      break;
    case 1: //24
      if (clockFrame == 2073600)
        clockFrame = 0;
      break;
    case 2: //25
      if (clockFrame == 2160000)
        clockFrame = 0;
      break;
    case 3: //29.97
      if (clockFrame == 2592000)
        clockFrame = 0;
      break;
    case 4: //29.97 DF
      if (clockFrame == 2592000)
        clockFrame = 0;
      if (clockFrame % 30 == 29 && (clockFrame / 30) % 60 == 59 && ((((clockFrame / 30) / 60) % 60)+1) % 10 != 0 )
        {
          clockFrame++; 
          clockFrame++;
        }
      break;
    case 5: //30
      if (clockFrame == 2592000)
        clockFrame = 0;
      break;
  }
}

/*

if (&clockFrame % 30 == 29 && (&clockFrame / 30) % 60 == 59 && ((((&clockFrame / 30) / 60) % 60)+1) % 10 != 0 )

*/