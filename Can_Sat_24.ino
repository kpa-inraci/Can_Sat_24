#include "VertiCan_TX.h"

#include "backup_managemet.h"
#include <Servo.h>

#define pin_servo_1 11

Servo MonServo1;
Servo MonServo2;

extern char sensor_type;
extern float erreur_x, erreur_y, erreur_z;
extern float ACCEL_XANGLE, ACCEL_YANGLE, ACCEL_ZANGLE;
extern unsigned long Time_ms;  // "temps" en milliseconde depuis le dernier reset du uP
extern float BMx280_AltitudeApprox;  // altitude
int compteur_regu = 0;
int compteur_donne = 0;
bool flag_altitude_start = 1; //attention a placer a zero pour le lancement ou ajouter une méthode de commande à distance
int statusCommand = 0;

int limite(int val, int lim_haute, int lim_basse) 
{
  if (val > lim_haute)
    return lim_haute;
  else if (val < lim_basse)
    return lim_basse;
  else
    return val;
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  MonServo1.attach(pin_servo_1);
  MonServo2.attach(pin_servo_2);

  init_backup_management();
  Wire.begin();
  mpu.initialize();
  init_RFM69();
  sensor_type = init_BMx280();
  /*attachAndWriteServo(Servomoteur1, SERVO_Pin_1, 0);
  attachAndWriteServo(Servomoteur2, SERVO_Pin_2, 0);*/
  init_flash();
  initPinIO(BUZZER_Pin, OUTPUT, LOW);
  buzzer_toggle(1000);
  Time_ms = millis();
  send_radio_msg("init done\n");
}

void loop() 
{
  //rfm69Reception();
  #ifdef DEBUG_radio
  Serial.printf("loop -> flag_altitude_start :%d\n", flag_altitude_start);
  #endif


  //status = backup_choice();
  if (millis() >= Time_ms + 10) {  //prends une mesure toute les 10ms
    get_data();

  statusCommand = commandeReception();
  if (millis() >= Time_ms + 10) 
  {  //prends une mesure toute les 10ms
    get_data();    
    Time_ms = millis();
    compteur_regu++;    compteur_donne++;
    // altitude max
  }

  if (BMx280_AltitudeApprox >= altitude_start_backup )   flag_altitude_start = true;  
    
  switch (statusCommand) 
  {
    case VERTICAN_format_file:
      formatMemory();
      Packetnum = 0;
      break;
    case VERTICAN_extract_file:
      extractData();
      waitAfterExtract(); //attention blocant
      break;
    case VERTICAN_backup_on_radio:
      send_flash_to_radio();
      waitAfterExtract(); //attention blocant
      break; 
    case VERTICAN_no_backup_on_flash:
      //fonction pour radio afficher !!noflash!!
      flag_altitude_start = false;    
      statusCommand = VERTICAN_run; //permet de reprendre le fonctionnement normal
      break;
    case VERTICAN_save_on_flash:
      flag_altitude_start = true;   
      statusCommand = VERTICAN_run;  //permet de reprendre le fonctionnement normal
      break;
    case VERTICAN_run:
      if (compteur_donne >= 10)  //x10 ms
      { 
        send_all_data(flag_altitude_start);
        compteur_donne = 0;
      }
      break;
  }
   if (compteur_regu >= 10) //x10ms
   {
    //erreur et objectif
    compteur_regu = 0;
    MonServo1.write(limite(erreur_y + 90, 120, 60));
    MonServo2.write(limite(erreur_y + 90, 120, 60));
    //regulation
  }
}
}






