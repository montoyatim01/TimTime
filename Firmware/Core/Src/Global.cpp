#include "Global.h"
/*Global Variables*/
uint32_t calibration = 23999979;
volatile uint8_t tcIN[8] = {0};         // Buffer to store valid TC data - sync bytes
volatile bool tcJammed = false;
uint8_t frameRate = 0;	//Global frame rate change

volatile uint32_t clockFrame = 0;

uint8_t intOffset = 30;

uint8_t autoOff = 0;
uint16_t autoOffMinutes = 60;
uint16_t uptimeMinutes = 0;
float batteryRemaining = 0.0;

uint8_t hr, mn, sc, fr = 0;

bool stat1, stat2;
bool buttonsHeld = false;
uint8_t lockCountdown = 0;

int32_t calibrationArray[6] = {0};
uint32_t calibrationInterval[6] = {240000,240000,250000,300000,300000,300000};
uint8_t frameRateDivisor[6] = {24,24,25,30,30,30};
uint16_t frameRateARR[6] = {50049,49999,47999,40039,40039,39999};
uint8_t powerCountdown = 0;



uint32_t displayTimeout = 0;