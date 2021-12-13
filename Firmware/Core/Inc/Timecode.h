#ifndef __TIMECODE_H
#define __TIMECODE_H

#else
bool initTimecode(uint32_t* clockFrame, uint8_t rate);
bool jamConfirmation();
bool timecodeBreakout(uint8_t* hour, 
                        uint8_t* min, 
                        uint8_t* sec, 
                        uint8_t* frame, 
                        uint32_t currentFrame, 
                        uint8_t rate);
bool resetTimecode();
void calibrate();
void frameCheck();
#endif