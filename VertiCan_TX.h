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
#include "MPU6050.h"

#define VERTICAN_format_file 1
#define VERTICAN_extract_file 2
#define VERTICAN_run 0


// Déclarations des #define originaux
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
#define backup_file
//#define printSerial_radiopack

// Déclarations des variables et des fonctions externes
extern float TMP36_Temperature, BMP280_Temperature, BMP280_Pression, BMP280_AltitudeApprox, BMx280_Hum;
extern float ACCEL_XANGLE, ACCEL_YANGLE, ACCEL_ZANGLE;
extern float x_out, y_out, z_out;
extern int Packetnum;
extern unsigned long Time_ms;
extern String Radiopacket;




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
void sendToSerial(uint16_t Packetnum,unsigned long Time_ms,float TMP36_Temperature,
  float BMP280_Temperature, float BMP280_Pression,  float BMP280_AltitudeApprox,  float BMx280_Hum, 
  float ACCEL_XANGLE, float ACCEL_YANGLE, float ACCEL_ZANGLE,
  float x_out, float y_out, float z_out);
char saveToFlash(uint16_t Packetnum, unsigned long Time_ms, float TMP36_Temperature, 
  float BMP280_Temperature, float BMP280_Pression, float BMP280_AltitudeApprox, float BMx280_Hum,  
  float ACCEL_XANGLE, float ACCEL_YANGLE, float ACCEL_ZANGLE, float x_out, float y_out, float z_out);
String creerRadioPacket(int Packetnum, long Time_ms, float TMP36_Temperature, 
  float BMP280_Temperature, float BMP280_Pression, float BMP280_AltitudeApprox, float BMx280_Hum,  
  float ACCEL_XANGLE, float ACCEL_YANGLE, float ACCEL_ZANGLE, float x_out, float y_out, float z_out);
void get_data(void);
void buzzer_toggle(unsigned int time); // Ajout de la déclaration de la fonction buzzer_toggle
void send_all_data(void);


#endif
