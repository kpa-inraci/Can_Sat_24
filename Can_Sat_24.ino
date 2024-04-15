#include "VertiCan_TX.h"

int compteur_regu = 0;
int compteur_donne = 0;
bool flag_altitude_start = true; //attention a placer a zero pour le lancement ou ajouter une méthode de commande à distance

int status = 0;

void setup() {
  delay(2000);
  Serial.begin(115200);

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

void loop() 
{
  
  status = commandeReception();
  if (millis() >= Time_ms + 10) 
  {  //prends une mesure toute les 10ms
    get_data();    
    Time_ms = millis();
    compteur_regu++;
    compteur_donne++;
    // altitude max
  }

  if (BMx280_AltitudeApprox >= altitude_start_backup || status == VERTICAN_save_on_flash)  
    {   
      flag_altitude_start = true;   
      status = VERTICAN_run;  //permet de reprendre le fonctionnement normal
    } 
  
  if (status == VERTICAN_no_backup_on_flash)  
    {    
      flag_altitude_start = false;    
      status = VERTICAN_run; //permet de reprendre le fonctionnement normal
    }
  
  switch (status) 
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
    case VERTICAN_run:
      if (compteur_donne >= 50)  //x10 ms
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
    //regulation
  }
}
