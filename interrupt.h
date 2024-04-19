#include <Arduino.h>
#include "Adafruit_ZeroTimer.h"


extern Adafruit_ZeroTimer zerotimer;

extern float freq; // 1 KHz -> 1ms
extern int cpt;

float smoothFilter(float current_angle, float last_filtered_angle);
void TC5_Handler(void); 
void TimerCallback0(void);
void init_interrupt(float);
