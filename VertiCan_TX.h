#ifndef VERTICAN_TX_H
#define VERTICAN_TX_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include <Servo.h>
#include <RH_RF69.h>  // Librairie du module radio RFM69
#include <Adafruit_BME280.h>
#include "backup_managemet.h"

#include "MPU6050.h"




// Déclarations des #define originaux
//#define DEBUG_radio
#define RFM69_FREQ 433.0  
#define RFM69_CS 6        
#define RFM69_INT 9       
#define RFM69_RST 10      
#define TMP36_Pin A3
#define BUZZER_Pin A5
#define SERVO_Pin_1 11
#define SERVO_Pin_2 12
#define FILE_NAME "data.csv"  
#define ALTITUDE_REF 1024.00  

#define VERTICAN_run 0
#define VERTICAN_format_file 1
#define VERTICAN_extract_file 2
#define VERTICAN_save_on_flash 3
#define VERTICAN_no_backup_on_flash 4
#define VERTICAN_backup_on_radio 5


#define consigne_x 0
#define consigne_y 0
#define consigne_z 0
#define backup_file
#define nb_packet 17
#define altitude_start_backup 500
#define sendAllData 0
typedef enum id_for_send_
{
  id_Packetnum = 1,
  id_Time_ms,
  id_TMP36_Temperature,
  id_BMP280_Temperature,
  id_BMP280_Pression,
  id_BMP280_AltitudeApprox,
  id_altitude_max,
  id_BMx280_Hum,
  id_ACCEL_XANGLE,
  id_erreur_x,
  id_x_out,
  id_ACCEL_YANGLE,
  id_erreur_y,
  id_y_out,
  id_ACCEL_ZANGLE,
  id_erreur_z,
  id_z_out
} id_for_send;
//#define printSerial_radiopack

// Déclarations des variables et des fonctions externes
extern float TMP36_Temperature, BMP280_Temperature, BMP280_Pression, BMP280_AltitudeApprox, BMx280_Hum;
extern float ACCEL_XANGLE, ACCEL_YANGLE, ACCEL_ZANGLE;
extern float x_out, y_out, z_out;
extern float erreur_x, erreur_y, erreur_z;
extern unsigned int Packetnum;
extern unsigned long Time_ms; // "temps" en milliseconde depuis le dernier reset du uP
extern String Radiopacket;
extern float altitude_max;

extern char sensor_type;
extern float BMx280_AltitudeApprox;  // altitude

extern uint8_t lenBuf_rfm69;
extern uint8_t buf_rfm69[RH_RF69_MAX_MESSAGE_LEN];






extern Adafruit_BMP280 BMP280;
extern FatFileSystem fatfs;
extern Servo Servomoteur1;
extern Servo Servomoteur2;
extern MPU6050 mpu;


char init_BME280(void);
char init_BMx280(void);
char get_BME280(void);
void get_BMx280(char);
char init_RFM69(void);
void attachAndWriteServo(Servo &servo, int pin, int angle);
void initPinIO(int pin, int mode, int value);
char init_flash(void);
char init_BMP280(void);
char get_BMP280(void);
void send_radio_msg(String);
String prep_data(int,float);
void get_data(void);
void buzzer_toggle(unsigned int time); // Ajout de la déclaration de la fonction buzzer_toggle
void send_all_data(bool activeWriteFlash);
char commandeReception(void);
String rfm69Reception(void); 
void send_flash_to_radio(void);
void waitAfterExtract(void);


void sendToSerial (uint16_t Packetnum,unsigned long Time_ms,float TMP36_Temperature,
  float BMP280_Temperature, float BMP280_Pression, float BMP280_AltitudeApprox,float altitude_max, float BMx280_Hum, 
  float ACCEL_XANGLE, float erreur_x, float x_out, 
  float ACCEL_YANGLE, float erreur_y,  float y_out,
  float ACCEL_ZANGLE, float erreur_z, float z_out);

char saveToFlash  (uint16_t Packetnum, unsigned long Time_ms, float TMP36_Temperature, 
  float BMP280_Temperature, float BMP280_Pression, float BMP280_AltitudeApprox,float altitude_max, float BMx280_Hum,  
  float ACCEL_XANGLE, float erreur_x, float x_out, 
  float ACCEL_YANGLE, float erreur_y,  float y_out,
  float ACCEL_ZANGLE, float erreur_z, float z_out);

void SendRadioPacket(uint16_t Packetnum,unsigned long Time_ms,float TMP36_Temperature,
  float BMP280_Temperature, float BMP280_Pression, float BMP280_AltitudeApprox,float altitude_max, float BMx280_Hum, 
  float ACCEL_XANGLE, float erreur_x, float x_out, 
  float ACCEL_YANGLE, float erreur_y,  float y_out,
  float ACCEL_ZANGLE, float erreur_z, float z_out);



#endif
