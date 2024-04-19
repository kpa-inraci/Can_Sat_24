#include "interrupt.h"
#include "VertiCan_TX.h"
 
// This example can have just about any frequency for the callback
// automatically calculated!
//float freq = 1000.0; // 1 KHz -> 1ms
int cpt = 0;


// timer tester
Adafruit_ZeroTimer zerotimer = Adafruit_ZeroTimer(5);


void TC5_Handler() 
{
  Adafruit_ZeroTimer::timerHandler(5);
}

// Fonction pour appliquer le filtre exponentiel
float smoothFilter(float current_angle, float last_filtered_angle) {
    const float alpha = 0.8;
    float filtered_angle = alpha * current_angle + (1 - alpha) * last_filtered_angle;
    return filtered_angle;
}

// the timer callback
volatile bool togglepin = false;
void TimerCallback0(void)
{
      angle_moteur_x = 0;
    angle_moteur_y = 0;
    
    float kpx = 0.65;
    float kdx = 0.25;
    float kpy = 0.65;
    float kdy = 0.25;
    static bool boolele = 0;
    static float ancien_angle_moteur_y;
    static float ancien_angle_moteur_x;
    if (boolele)
    {//todo corriger pid facteur d
      boolele = 0;
      Ax = erreur_x;
      Ay = erreur_y;
    }
    else{
      Bx = erreur_x;
      By = erreur_y;
      deltax = Ax - Bx;
      deltay = Ay - By;
      boolele = 1;

    }
    //erreur et objectif
    //compteur_regu = 0;

// Filtrer les changements brusques d'angle
// Définition des constantes pour le filtre
float last_filtered_angle_x = 90;
float last_filtered_angle_y = 90;



// Fonction pour limiter les valeurs entre deux bornes

// À l'intérieur de votre boucle principale ou de votre routine d'interruption :
// Appliquer le filtre à l'angle de moteur x et y
angle_moteur_x = 90 - (int)(erreur_x * kpx + deltax * kdx);
angle_moteur_y = (int)(erreur_y * kpy + deltay * kdy) + 90;

float filtered_angle_x = smoothFilter(angle_moteur_x, last_filtered_angle_x);
float filtered_angle_y = smoothFilter(angle_moteur_y, last_filtered_angle_y);

// Écrire les angles filtrés sur les servomoteurs en respectant les limites
MonServo1.write(limite((int)filtered_angle_y, 120, 60));
MonServo2.write(limite((int)filtered_angle_x, 120, 60));

// Mettre à jour les variables pour la prochaine itération
last_filtered_angle_x = filtered_angle_x;
last_filtered_angle_y = filtered_angle_y;

// Calculer les nouveaux angles
// Limiter les angles dans une plage acceptable
//int angle_limite_x = limite(angle_moteur_x, 120, 60);
//int angle_limite_y = limite(angle_moteur_y, 120, 60);

// Mettre à jour les servomoteurs avec les angles limités


}

void init_interrupt(float freq)
{
  Serial.println("Timer callback tester");
 
  Serial.print("Desired freq (Hz):");
  Serial.println(freq);
 
  // Set up the flexible divider/compare
  uint16_t divider  = 1;
  uint16_t compare = 0;
  tc_clock_prescaler prescaler = TC_CLOCK_PRESCALER_DIV1;
 
  if ((freq < 24000000) && (freq > 800)) {
    divider = 1;
    prescaler = TC_CLOCK_PRESCALER_DIV1;
    compare = 48000000/freq;
  } else if (freq > 400) {
    divider = 2;
    prescaler = TC_CLOCK_PRESCALER_DIV2;
    compare = (48000000/2)/freq;
  } else if (freq > 200) {
    divider = 4;
    prescaler = TC_CLOCK_PRESCALER_DIV4;
    compare = (48000000/4)/freq;
  } else if (freq > 100) {
    divider = 8;
    prescaler = TC_CLOCK_PRESCALER_DIV8;
    compare = (48000000/8)/freq;
  } else if (freq > 50) {
    divider = 16;
    prescaler = TC_CLOCK_PRESCALER_DIV16;
    compare = (48000000/16)/freq;
  } else if (freq > 12) {
    divider = 64;
    prescaler = TC_CLOCK_PRESCALER_DIV64;
    compare = (48000000/64)/freq;
  } else if (freq > 3) {
    divider = 256;
    prescaler = TC_CLOCK_PRESCALER_DIV256;
    compare = (48000000/256)/freq;
  } else if (freq >= 0.75) {
    divider = 1024;
    prescaler = TC_CLOCK_PRESCALER_DIV1024;
    compare = (48000000/1024)/freq;
  } else {
    Serial.println("Invalid frequency");
    while (1) delay(10);
  }
  Serial.print("Divider:"); Serial.println(divider);
  Serial.print("Compare:"); Serial.println(compare);
  Serial.print("Final freq:"); Serial.println((int)(48000000/compare));
 
  zerotimer.enable(false);
  zerotimer.configure(prescaler,       // prescaler
          TC_COUNTER_SIZE_16BIT,       // bit width of timer/counter
          TC_WAVE_GENERATION_MATCH_PWM // frequency or PWM mode
          );
 
  zerotimer.setCompare(0, compare);
  zerotimer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, TimerCallback0);
  zerotimer.enable(true);
}

 