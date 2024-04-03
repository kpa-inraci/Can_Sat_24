#include "VertiCan_TX.h"
#include "backup_managemet.h"
extern char sensor_type;
extern unsigned long Time_ms;  // "temps" en milliseconde depuis le dernier reset du uP
int compteur_regu = 0;
int compteur_donne = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(2000);
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
void loop() {
  Serial1.write(178);
  status = backup_choice();
  if (millis() >= Time_ms + 10) {  //prends une mesure toute les 10ms
    get_data();
    Time_ms = millis();
    compteur_regu++;
    compteur_donne++;
  }
  if (compteur_regu >= 10) {
    compteur_regu = 0;
    //regulation

  }


  switch (status) {
    case VERTICAN_format_file:
      formatMemory();
      Packetnum = 0;
      break;
    case VERTICAN_extract_file:
      extractData();
      Serial.println("data has been extracted enter something to run");
      while (1) {
        if (Serial.available()) { break; }
      }
      break;
    case VERTICAN_run:
      if (compteur_donne >= 100) {
        Packetnum++;
        send_all_data();
        compteur_donne = 0;
      }
      break;
  }
}
