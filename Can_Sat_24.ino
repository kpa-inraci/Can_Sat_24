#include "VertiCan_TX.h"
#include "backup_managemet.h"
extern char sensor_type;

void setup() 
{
  Serial.begin(115200);
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
char status=0;
void loop() 
{
 
  status=backup_choice();

 
  if (millis() >= Time_ms + 100) 
  {
    Time_ms = millis();  
    get_data();  
    Packetnum++;   

    switch (status)
    {
      case VERTICAN_format_file:  formatMemory(); Packetnum = 0;		break;
      case VERTICAN_extract_file: extractData(); Serial.println("data has been extracted enter something to run");while(1){if (Serial.available()){break;}}	  break;
      case VERTICAN_run:          send_all_data();	break;
      
    }
  }
}


