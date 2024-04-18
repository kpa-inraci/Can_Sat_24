#include "VertiCan_TX.h"

#define pin_servo_x 11
#define pin_servo_y 12

int compteur_regu = 0;
int compteur_donne = 0;
bool flag_altitude_start = 0; //attention a placer a zero pour le lancement ou ajouter une méthode de commande à distance
int statusCommand = 0;
float deltax;
float deltay;
float Ay;
float Ax;
float Bx;
float By;
int angle_moteur_x;
int angle_moteur_y;

Servo MonServo1;
Servo MonServo2;

void setup() {
  delay(2000);
  Serial.begin(115200);
  
  init_backup_management();
  Wire.begin();
  mpu.initialize();
  init_RFM69();
  sensor_type = init_BMx280();
 // attachAndWriteServo(Servomoteur1, pin_servo_1, 0);
  //attachAndWriteServo(Servomoteur2, pin_servo_2, 0);
  init_flash();
  initPinIO(BUZZER_Pin, OUTPUT, LOW);
  //buzzer_toggle(1000);
  Time_ms = millis();
  send_radio_msg("init done\n");
  MonServo1.attach(pin_servo_x);
  MonServo2.attach(pin_servo_y);
  MonServo1.write(90);
  MonServo2.write(90);
  //delay(700);
  init_interrupt(10.0);
  TimerCallback0();
 
}

void loop() 
{
  //buzzer_toggle(500);
  //led_toggle(200);
  //send_radio_msg(String(Serial.read()));
  //rfm69Reception();
  #ifdef DEBUG_radio
    Serial.printf("loop -> flag_altitude_start :%d\n", flag_altitude_start);
  #endif

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
      while(send_flash_to_radio()){
      }
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
      if (compteur_donne >= 50)  //x10 ms
      { 
        send_all_data(flag_altitude_start);
        compteur_donne = 0;
      }
      break;
  }
   if (compteur_regu >= 10) //x10ms
   {
    Serial.printf("erreur dx =%f  erreur dy =%f\n", deltax, deltay);
    //if (altitude_max - BMx280_AltitudeApprox >= 200)
    //{
      
    //}
    
    //Serial.printf("angle_moteur_y = %d angle_moteur_x = %d cpt = %d\n", angle_moteur_y, angle_moteur_x, cpt);
    //regulation
  }

}
