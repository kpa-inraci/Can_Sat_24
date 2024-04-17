#include "WString.h"
#include "Arduino.h"
#include "VertiCan_TX.h"
#include "backup_managemet.h"


String tableau[nb_packet + 1];


char sensor_type = 0;
float TMP36_Temperature;
float BMx280_Hum = 0;
float BMx280_Temperature;
float BMx280_Pression;
float BMx280_AltitudeApprox;
float erreur_x = 0;
float erreur_y = 0;
float erreur_z = 0;
float altitude_max = 0;
float ancienne_altitude = 0;  //alan altitude max alan
unsigned long Time_ms;  // "temps" en milliseconde depuis le dernier reset du uP
unsigned int Packetnum = 0;            // Numéro du paquet de donnée. Sera incrémenté à chaque envoi
String Radiopacket;           // Paquet de donnée qui sera transmis à la station de base

/*source : MPU6050.cpp*/ extern float ACCEL_XANGLE, ACCEL_YANGLE, ACCEL_ZANGLE;  //bgh Déclaration des variables globales
/*source : MPU6050.cpp*/ extern float x_out, y_out, z_out;                       //bgh Declaration des acceleration lineaire en g



//************* DEFINITION DES OBJETS ************
RH_RF69 rfm69(RFM69_CS, RFM69_INT);
Adafruit_BMP280 BMP280;
Adafruit_BME280 BME280;
Servo Servomoteur1;
Servo Servomoteur2;
extern MPU6050 mpu;


// Définition de la fonction buzzer_toggle
void buzzer_toggle(unsigned int time) 
{
  digitalWrite(BUZZER_Pin, 1);
  delay(time);
  digitalWrite(BUZZER_Pin, 0);
  delay(time);
}

void attachAndWriteServo(Servo &servo, int pin, int angle) {
  servo.attach(pin);
  servo.write(angle);
}

void initPinIO(int pin, int mode, int value) {
  pinMode(pin, mode);
  digitalWrite(pin, value);
}


void send_all_data(bool activeWriteFlash) 
{

  Packetnum++; //incremente le compteur a chaque passage

                  SendRadioPacket(Packetnum, Time_ms,
                                 TMP36_Temperature,
                                 BMx280_Temperature, BMx280_Pression, BMx280_AltitudeApprox, altitude_max, BMx280_Hum,
                                 ACCEL_XANGLE, erreur_x, x_out,
                                 ACCEL_YANGLE, erreur_y, y_out,
                                 ACCEL_ZANGLE, erreur_z, z_out);
  // Sauvegarde des mesures dans la mémoire flash
#ifdef backup_file

  #ifdef DEBUG_radio
  Serial.printf("send_all_data -> activeWriteFlash :%d", activeWriteFlash);
  #endif

  if(activeWriteFlash)
  {
    saveToFlash(Packetnum, Time_ms,
                TMP36_Temperature,
                BMx280_Temperature, BMx280_Pression, BMx280_AltitudeApprox, altitude_max, BMx280_Hum,
                ACCEL_XANGLE, erreur_x, x_out,
                ACCEL_YANGLE, erreur_y, y_out,
                ACCEL_ZANGLE, erreur_z, z_out);
  }
  else 
  {  
    Serial.println("NO Flash backUP !!!!!!!!!");
    send_radio_msg("no backup on flash\n");
    //envoyer par radio 
  }
    
  #else
    Serial.println("NO Flash backUP !!!!!!!!!");
  #endif

  // Envoi du radiopacket à la station de base. ATTENTION : bloquant si module radio rrfm69 absent !!
  /*rfm69.send((uint8_t *)(Radiopacket.c_str()), Radiopacket.length());
    rfm69.waitPacketSent();*/


  sendToSerial(Packetnum, Time_ms,
               TMP36_Temperature,
               BMx280_Temperature, BMx280_Pression, BMx280_AltitudeApprox, altitude_max, BMx280_Hum,
               ACCEL_XANGLE, erreur_x, x_out,
               ACCEL_YANGLE, erreur_y, y_out,
               ACCEL_ZANGLE, erreur_z, z_out);
}

char init_RFM69(void) {
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // Initialisation et configuration du module radio RFM69
  digitalWrite(RFM69_RST, HIGH);  // Reset manuel
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  if (!rfm69.init()) {
    Serial.println("RFM69 radio init failed");
    //while (1);        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ATTENTION BLOQUANT !!!!!!!!!!!!!!!!!!!!!!
    return 1;
  }
  Serial.println("RFM69 radio init OK!");
  if (!rfm69.setFrequency(RFM69_FREQ)) {
    Serial.println("setFrequency failed");
    return 1;
    //while (1);        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ATTENTION BLOQUANT !!!!!!!!!!!!!!!!!!!!!!
  }
  rfm69.setTxPower(20, true);  // Puissance d'émission 14 à 20
  // Clé de codage doit être identique dans le récepteur
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  rfm69.setEncryptionKey(key);

  return 0;
}


char init_BMx280() {
  if (!init_BMP280()) {

    Serial.println("c'est un BMP");
    return 'P';

  } else if (init_BME280()) {
    Serial.println("C'est un BME");
    return 'E';
  }
  return 0;
}
char init_BME280(void) {
  if (BME280.begin()) {
    Serial.println("Erreur connexion capteur BME280");
    return 1;
    //while (1);   // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ATTENTION BLOQUANT !!!!!!!!!!!!!!!!!!!!!!
  }
  /* Default settings from datasheet. */
  BME280.setSampling(Adafruit_BME280::MODE_NORMAL,     /* Operating Mode. */
                     Adafruit_BME280::SAMPLING_X2,     /* Temp. oversampling */
                     Adafruit_BME280::SAMPLING_X16,    /* Pressure oversampling */
                     Adafruit_BME280::SAMPLING_X16,    /*hum sampling*/
                     Adafruit_BME280::FILTER_X16,      /* Filtering. */
                     Adafruit_BME280::STANDBY_MS_500); /* Standby time. */
  return 0;
}
char init_BMP280(void) {
  if (!BMP280.begin()) {
    Serial.println("Erreur connexion capteur BMP280");
    return 1;
    //while (1);   // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ATTENTION BLOQUANT !!!!!!!!!!!!!!!!!!!!!!
  }
  /* Default settings from datasheet. */
  BMP280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                     Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                     Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                     Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                     Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  return 0;
}
char init_flash(void) {
  if (!flash.begin()) {
    Serial.println("Error, failed to initialize flash chip!");
    return 1;
  }

  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    Serial.println("Was the flash chip formatted with the fatfs_format example?");
    return 1;
  }
  return 0;
}
void get_data(void) {
  TMP36_Temperature = (((analogRead(TMP36_Pin) * 3.3) / 1024) - 0.5) * 100;
  get_BMx280(sensor_type);
  Get_Accel_Angles();
  ancienne_altitude = BMx280_AltitudeApprox;
  if (BMx280_AltitudeApprox > altitude_max) {
    altitude_max = BMx280_AltitudeApprox;
  }
  erreur_x = ACCEL_XANGLE - consigne_x;
  erreur_y = ACCEL_YANGLE - consigne_y;
  erreur_z = ACCEL_ZANGLE - consigne_z;
}
void get_BMx280(char modele) {
  if (modele == 'P')
    get_BMP280();
  if (modele == 'E')
    get_BME280();
}
char get_BME280(void) {
  BMx280_Temperature = BME280.readTemperature();
  BMx280_Pression = BME280.readPressure();
  BMx280_AltitudeApprox = BME280.readAltitude(ALTITUDE_REF);
  BMx280_Hum = BME280.readHumidity();
  return 0;
}
char get_BMP280(void) {
  BMx280_Temperature = BMP280.readTemperature();
  BMx280_Pression = BMP280.readPressure();
  BMx280_AltitudeApprox = BMP280.readAltitude(ALTITUDE_REF);
  return 0;
}
String prep_data(int data1, float data2) 
{
  return data1 + String(',') + data2 + String(',');  //+ String((float)data1+data2);
}

void SendRadioPacket(uint16_t Packetnum, unsigned long Time_ms, float TMP36_Temperature,
                        float BMP280_Temperature, float BMP280_Pression, float BMP280_AltitudeApprox, float altitude_max, float BMx280_Hum,
                        float ACCEL_XANGLE, float erreur_x, float x_out,
                        float ACCEL_YANGLE, float erreur_y, float y_out,
                        float ACCEL_ZANGLE, float erreur_z, float z_out) {
  tableau[0] = String('#') + nb_packet + String(';');
  tableau[1] = prep_data(id_Packetnum, Packetnum);
  tableau[2] = prep_data(id_Time_ms, Time_ms);
  tableau[3] = prep_data(id_TMP36_Temperature, TMP36_Temperature);
  tableau[4] = prep_data(id_BMP280_Temperature, BMP280_Temperature);
  tableau[5] = prep_data(id_BMP280_Pression, BMP280_Pression);
  tableau[6] = prep_data(id_BMP280_AltitudeApprox, BMP280_AltitudeApprox);
  tableau[7] = prep_data(id_altitude_max, altitude_max);
  tableau[8] = prep_data(id_BMx280_Hum, BMx280_Hum);
  tableau[9] = prep_data(id_ACCEL_XANGLE, ACCEL_XANGLE);
  tableau[10] = prep_data(id_erreur_x, erreur_x);
  tableau[11] = prep_data(id_x_out, x_out);
  tableau[12] = prep_data(id_ACCEL_YANGLE, ACCEL_YANGLE);
  tableau[13] = prep_data(id_erreur_y, erreur_y);
  tableau[14] = prep_data(id_y_out, y_out);
  tableau[15] = prep_data(id_ACCEL_ZANGLE, ACCEL_ZANGLE);
  tableau[16] = prep_data(id_erreur_z, erreur_z);
  tableau[17] = prep_data(id_z_out, z_out) + String('$');


  for (char cptSend = 0; cptSend < nb_packet + 1; cptSend++) {
    rfm69.send((uint8_t *)(tableau[cptSend].c_str()), tableau[cptSend].length()); /*envois*/
    rfm69.waitPacketSent();
    delayMicroseconds(50 * sizeof(tableau));  //va niquer ta mère
  }
}
char saveToFlash(uint16_t Packetnum, unsigned long Time_ms, float TMP36_Temperature,
                 float BMP280_Temperature, float BMP280_Pression, float BMP280_AltitudeApprox, float altitude_max, float BMx280_Hum,
                 float ACCEL_XANGLE, float erreur_x, float x_out,
                 float ACCEL_YANGLE, float erreur_y, float y_out,
                 float ACCEL_ZANGLE, float erreur_z, float z_out) {
  File dataFile = fatfs.open(FILE_NAME, FILE_WRITE);  // Ouvre le fichier pour l'écriture
  if (dataFile) {                                     // Vérifie si l'ouverture du fichier a réussi
    dataFile.print("#");
    dataFile.print(nb_packet);
    dataFile.print(",");
    dataFile.print(Packetnum);
    dataFile.print(",");
    dataFile.print(Time_ms);
    dataFile.print(",");
    dataFile.print(TMP36_Temperature);
    dataFile.print(",");
    dataFile.print(BMP280_Temperature);
    dataFile.print(",");
    dataFile.print(BMP280_Pression);
    dataFile.print(",");
    dataFile.print(BMP280_AltitudeApprox);
    dataFile.print(",");
    dataFile.print(altitude_max);
    dataFile.print(",");
    dataFile.print(BMx280_Hum);
    dataFile.print(",@");
    dataFile.print(ACCEL_XANGLE);
    dataFile.print(",");
    dataFile.print(erreur_x);
    dataFile.print(",");
    dataFile.print(x_out);
    dataFile.print(",");
    dataFile.print(ACCEL_YANGLE);
    dataFile.print(",");
    dataFile.print(erreur_y);
    dataFile.print(",");
    dataFile.print(y_out);
    dataFile.print(",");
    dataFile.print(ACCEL_ZANGLE);
    dataFile.print(",");
    dataFile.print(erreur_z);
    dataFile.print(",");
    dataFile.print(z_out);
    dataFile.println("$");

    dataFile.close();  // Ferme le fichier
    return 0;
  } else {
    Serial.println("Failed to open data file for writing!");  // Affiche un message en cas d'échec de l'ouverture du fichier
    return 1;
  }
}
void sendToSerial(uint16_t Packetnum, unsigned long Time_ms, float TMP36_Temperature,
                  float BMP280_Temperature, float BMP280_Pression, float BMP280_AltitudeApprox, float altitude_max, float BMx280_Hum,
                  float ACCEL_XANGLE, float erreur_x, float x_out,
                  float ACCEL_YANGLE, float erreur_y, float y_out,
                  float ACCEL_ZANGLE, float erreur_z, float z_out) {
  //temperature, pression, humidité, altitude
  Serial.printf("Packet :%8d", Packetnum);
  Serial.printf(" Time_ms :%8d", Time_ms);
  Serial.printf(" TMP36_T°= %4.1f", TMP36_Temperature);
  Serial.printf("  BMx280_T°= %4.1f", BMP280_Temperature);
  Serial.printf("  BMx280_P°= %10.3fPa", BMP280_Pression);
  Serial.printf("  BMx280_Alti= %6.2fm", BMP280_AltitudeApprox);
  Serial.printf("  BMx280_Alti_max= %d", int(altitude_max));
  Serial.printf("  BMx280_Hum= %4.1f\n", BMx280_Hum);
  //angle, pid, acceleration
  Serial.printf("Axe X:angle=%6.2f° erreur=%3.1f acc=%6.3f", ACCEL_XANGLE, erreur_x, x_out);
  Serial.printf("  Axe Y:angle=%6.2f° erreur=%3.1f acc=%6.3f", ACCEL_YANGLE, erreur_y, y_out);
  Serial.printf("  Axe Z:angle=%6.2f° erreur=%3.1f acc=%6.3f\n", ACCEL_ZANGLE, erreur_z, z_out);
#ifdef printSerial_radiopack
  Serial.print("Radiopacket : ");
  Serial.print(Radiopacket);
#endif
}

char commandeReception() 
{
  static String commandBuffer = ""; // Variable statique pour stocker la commande en cours de saisie
  char receivedChar;

  String receivedData = rfm69Reception(); // Stocker la donnée reçue dans une variable de type String

  if (strstr(receivedData.c_str(), "format") != NULL) // Recherche du mot "format" dans le tampon 
      {
        Serial.println("Le mot 'format' a été recu en RF!");
           return VERTICAN_format_file; 
      }

    if (strstr(receivedData.c_str(), "extract") != NULL)  // Recherche du mot "extract" dans le tampon
      {
        Serial.println("Le mot 'extract' a été recu en RF!");
        return VERTICAN_extract_file;
      }

      if (strstr(receivedData.c_str(), "save") != NULL)  // Recherche du mot "save" dans le tampon
      {
        Serial.println("Le mot 'save' a été recu en RF!");
        return VERTICAN_save_on_flash;
      }

      if (strstr(receivedData.c_str(), "noflash") != NULL)  // Recherche du mot "noflash" dans le tampon
      {
        Serial.println("Le mot 'noflash' a été recu en RF!");
        return VERTICAN_no_backup_on_flash;
      }

      if (strstr(receivedData.c_str(), "radio") != NULL)  // Recherche du mot "radio" dans le tampon
      {
        Serial.println("Le mot 'radio' a été recu en RF!");
        return VERTICAN_backup_on_radio;
      }
  
  while (Serial.available() > 0 || commandBuffer.length() > 0 ) 
  {
    while (Serial.available() > 0)  // Lire les caractères disponibles depuis le port série
    {
      receivedChar = Serial.read();
  
      if (receivedChar == ' ' && commandBuffer.length() == 0)   // Ignorer les caractères d'espacement supplémentaires
      {
        continue;
      }

      if (receivedChar == '\r' || receivedChar == '\n')         // Si le caractère est un retour chariot ou un retour à la ligne
      {
        if (commandBuffer.length() > 0) // Traiter la commande // Vérifier si la commande est non vide
        { 
          commandBuffer.trim(); // Supprimer les espaces avant et après la commande
          if (commandBuffer == "format") 
          {
            if (confirmFormat()) 
            {
              commandBuffer = ""; // Réinitialiser le tampon de commande
              return VERTICAN_format_file;
            } 
          } 
          else if (commandBuffer == "extract")
          {
            Serial.println("data extract file");
            commandBuffer = ""; // Réinitialiser le tampon de commande
            return VERTICAN_extract_file;
            //extractData();
          }  
          else if (commandBuffer == "save")
          {
            commandBuffer = ""; // Réinitialiser le tampon de commande
            return VERTICAN_save_on_flash;
          }  
          else if (commandBuffer == "noflash")
          {
            commandBuffer = ""; // Réinitialiser le tampon de commande
            return VERTICAN_no_backup_on_flash;
          } 
          else if (commandBuffer == "radio")
          {
            commandBuffer = ""; // Réinitialiser le tampon de commande
            return VERTICAN_backup_on_radio;
          }
          else 
          {
            Serial.println("Invalid command. Type 'format' to format the flash memory or 'extract' to extract data.");
          }
        }
        commandBuffer = "";             // Réinitialiser le tampon de commande
      } else 
      {
        commandBuffer += receivedChar;  // Ajouter le caractère au tampon de commande
      }
    }
  }
  return VERTICAN_run;
}
String rfm69Reception() 
{
  uint8_t len;
  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  
  if (rfm69.available()) // Vérifier si des données sont disponibles
  {
    len = sizeof(buf);
    if (rfm69.recv(buf, &len)) 
    {
      // Assurer qu'il y a des données dans le tampon
      if (len > 0) 
      {
        Serial.print("donnée dans le tampon RF :");
        Serial.println((char*)buf);  

        // Convertir le tampon en une chaîne de caractères
        buf[len] = '\0'; // Assurer que le tampon est terminé par un caractère nul pour être une chaîne valide
        return String((char*)buf);
      }
    }
  }
  // Si aucune donnée n'est disponible, retourner une chaîne vide
  return "";
}

void waitAfterExtract(void)
{
  bool breakWhile=0;
  String message="data has been extracted enter something to run";
   while (1)
      {
        for (uint8_t i = 0; i < 60; i++)
        {
          delay(5);
          if(Serial.available()  || rfm69Reception() !="")  { breakWhile=1; break;} 
        }
        if(breakWhile) break;   //permet de sortir de laboucle while(1)
        Serial.println(message.c_str());     //print du status sur le terminal         
        send_radio_msg(message);
      }
}

void send_flash_to_radio(void)
{
  #define FILE_NAME "data.csv"
  File dataFile = fatfs.open(FILE_NAME, FILE_READ);

    rfm69.send((uint8_t *)'?', 1); //permet de retrouver le status via la radio
    rfm69.waitPacketSent();
    while (dataFile.available())
    {
      char c = dataFile.read(); // Lire un caractère (type char)
      rfm69.send((uint8_t *)&c, 1); /* Envois */
      rfm69.waitPacketSent();
      delayMicroseconds(10);
      }
  rfm69.send((uint8_t *)'!', 1); //permet de retrouver le status via la radio
  rfm69.waitPacketSent();
  // ? debut ! fin
}

void send_radio_msg(String message)
{
  rfm69.send((uint8_t *)(message.c_str()), message.length()); //permet de retrouver le status via la radio
  rfm69.waitPacketSent();
}

