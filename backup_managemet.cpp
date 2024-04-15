#include "backup_managemet.h"
//#include "VertiCan_TX.h"

#if defined(__SAMD51__) || defined(NRF52840_XXAA)
  Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS, PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
#else
  #if (SPI_INTERFACES_COUNT == 1 || defined(ADAFRUIT_CIRCUITPLAYGROUND_M0))
    Adafruit_FlashTransport_SPI flashTransport(SS, &SPI);
  #else
    Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
  #endif
#endif


Adafruit_SPIFlash flash(&flashTransport);
FatFileSystem fatfs;


char init_backup_management()
{
  Serial.println("start init backup soft");
  if (!flash.begin())
  {
    Serial.println("Error, failed to initialize flash chip!");
    //while(1); //TODO fixe me
    return 1;
  }
  Serial.print("Flash chip JEDEC ID: 0x"); Serial.println(flash.getJEDECID(), HEX);

  Serial.setTimeout(30000);
  Serial.println("Type 'format' to format the flash memory or 'extract' to extract data.");
  return 0;
}




void formatMemory() {
  uint8_t buf[512] = {0};
  FRESULT r = f_mkfs("", FM_FAT | FM_SFD, 0, buf, sizeof(buf));
  Serial.println("start format file");
  if (r != FR_OK) {
    Serial.print("Error, f_mkfs failed with error code: "); Serial.println(r, DEC);
    return;
  }

  flash.syncBlocks();

  Serial.println("Formatted flash!");
}

void extractData() 
{
  if (!fatfs.begin(&flash)) 
  {
    Serial.println("Error, failed to mount filesystem!");
    return;
  }

  File dataFile = fatfs.open(FILE_NAME, FILE_READ);
  if (dataFile) 
  {
    Serial.println("Opened file, printing contents below:");
    while (dataFile.available()) {
      char c = dataFile.read();
      Serial.print(c);
    }
    dataFile.close();
  } else {
    Serial.println("Failed to open data file! Does it exist?");
  }

}

void extractDataToBase() {
  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount filesystem!");
    return;
  }

File dataFile = fatfs.open(FILE_NAME, FILE_READ);
if (dataFile) {
  Serial.println("Opened file, printing contents below:");
  while (dataFile.available()) {
    char c = dataFile.read(); // Lire un caractère (type char)
    rfm69.send((uint8_t *)&c, 1); /* Envois */
    rfm69.waitPacketSent();
    delayMicroseconds(10);
    Serial.print(c);
  }
  dataFile.close();
} else {
  Serial.println("Failed to open data file! Does it exist?");
}


}


bool confirmFormat()
{
  Serial.println("Are you sure you want to format the flash memory? This will erase all data. (yes/no)");
  String response = ""; // Variable pour stocker la réponse

  while (true) {
    // Lire les caractères série jusqu'à ce qu'un retour chariot ou un retour à la ligne soit détecté
    while (Serial.available() > 0) {
      char receivedChar = Serial.read();

      // Ignorer les caractères d'espacement supplémentaires
      if (receivedChar == ' ' && response.length() == 0) {
        continue;
      }

      // Si le caractère est un retour chariot ou un retour à la ligne
      if (receivedChar == '\r' || receivedChar == '\n') {
        // Vérifier si la réponse est non vide
        if (response.length() > 0) {
          response.trim(); // Supprimer les espaces avant et après la réponse
          if (response.equalsIgnoreCase("yes")) 
          {
            return true;
          } 
          else if (response.equalsIgnoreCase("no")) 
          {
            Serial.println("Format canceled.");
            return false;
          } else {
            Serial.println("Invalid response. Please enter 'yes' or 'no'.");
          }
        }
        // Réinitialiser la réponse
        response = "";
      } else {
        // Ajouter le caractère à la réponse
        response += receivedChar;
      }
    }
  }
}
extern "C"
{
  DSTATUS disk_status ( BYTE pdrv ) {
    (void) pdrv;
    return 0;
  }

  DSTATUS disk_initialize ( BYTE pdrv ) {
    (void) pdrv;
    return 0;
  }

  DRESULT disk_read (
    BYTE pdrv,
    BYTE *buff,
    DWORD sector,
    UINT count
  ) {
    (void) pdrv;
    return flash.readBlocks(sector, buff, count) ? RES_OK : RES_ERROR;
  }

  DRESULT disk_write (
    BYTE pdrv,
    const BYTE *buff,
    DWORD sector,
    UINT count
  ) {
    (void) pdrv;
    return flash.writeBlocks(sector, buff, count) ? RES_OK : RES_ERROR;
  }

  DRESULT disk_ioctl (
    BYTE pdrv,
    BYTE cmd,
    void *buff
  ) {
    (void) pdrv;

    switch ( cmd ) {
      case CTRL_SYNC:
        flash.syncBlocks();
        return RES_OK;

      case GET_SECTOR_COUNT:
        *((DWORD*) buff) = flash.size()/512;
        return RES_OK;

      case GET_SECTOR_SIZE:
        *((WORD*) buff) = 512;
        return RES_OK;

      case GET_BLOCK_SIZE:
        *((DWORD*) buff) = 8;
        return RES_OK;

      default:
        return RES_PARERR;
    }
  }
}
