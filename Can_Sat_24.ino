#include "VertiCan_TX.h"
#include "backup_managemet.h"
extern char sensor_type;
extern float erreur_x, erreur_y, erreur_z;
extern float ACCEL_XANGLE, ACCEL_YANGLE, ACCEL_ZANGLE;
extern unsigned long Time_ms;  // "temps" en milliseconde depuis le dernier reset du uP
extern float BMx280_AltitudeApprox;  // altitude
int compteur_regu = 0;
int compteur_donne = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  init_backup_management();
  Wire.begin();
  mpu.initialize();
  init_RFM69();
  sensor_type = init_BMx280();
  attachAndWriteServo(Servomoteur1, SERVO_Pin_1, 0);
  attachAndWriteServo(Servomoteur2, SERVO_Pin_2, 0);
  init_flash();
  initPinIO(BUZZER_Pin, OUTPUT, LOW);
  initPinIO(LED, OUTPUT, LOW);
  buzzer_toggle(1000);
  Time_ms = millis();
}
char status = 0;
void loop() 
{

  LED_toggle(200);
  status = backup_choice();
  if (millis() >= Time_ms + 10) {  //prends une mesure toute les 10ms
    get_data();    
    Time_ms = millis();
    compteur_regu++;
    compteur_donne++;
    // altitude max
  }
 


  switch (status) 
  {
    case VERTICAN_format_file:
      formatMemory();
      Packetnum = 0;
      break;
    case VERTICAN_extract_file:
      extractData();
    
      while (!Serial.available())
      {
        for (uint8_t i = 0; i < 60; i++)
        {
          delay(5);
          if(Serial.available())
            break;
        }
        Serial.println("data has been extracted enter something to run");
      }
      break; 
    case VERTICAN_run:
      if (compteur_donne >= 50) 
      {
        Packetnum++;
        send_all_data();
        compteur_donne = 0;
      }
      break;
  }
   if (compteur_regu >= 10) 
   {
    //erreur et objectif
    compteur_regu = 0;
    //regulation
  }
}
