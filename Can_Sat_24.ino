#include "VertiCan_TX.h"
#include "backup_managemet.h"
extern char sensor_type;
extern float erreur_x, erreur_y, erreur_z;
extern float ACCEL_XANGLE, ACCEL_YANGLE, ACCEL_ZANGLE;
extern unsigned long Time_ms;  // "temps" en milliseconde depuis le dernier reset du uP
extern float BMx280_AltitudeApprox;  // altitude
int compteur_regu = 0;
int compteur_donne = 0;
float altitude_max = 0;
float ancienne_altitude = 0;

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
  buzzer_toggle(1000);
  Time_ms = millis();
}
char status = 0;
void loop() 
{
  status = backup_choice();
  if (millis() >= Time_ms + 10) {  //prends une mesure toute les 10ms
    get_data();

     ancienne_altitude = BMx280_AltitudeApprox;
    if (BMx280_AltitudeApprox > altitude_max) {
      altitude_max = BMx280_AltitudeApprox;
    }

    
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
        Serial.printf("altitude %d\t", int(BMx280_AltitudeApprox));
        Serial.printf("altitude max %d\t", int(altitude_max));
        Serial.printf("altitude ancienne %d\n", int(ancienne_altitude));
        Serial.printf("ex =%3.1f, ax =%3.1f", erreur_x, ACCEL_XANGLE);
        Serial.printf("ey =%3.1f, ay =%3.1f", erreur_y, ACCEL_YANGLE);
        Serial.printf("ez =%3.1f, az =%3.1f\n", erreur_z, ACCEL_ZANGLE);
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
