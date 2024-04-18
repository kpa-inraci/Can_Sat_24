#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include "ff.h"
#include "diskio.h"
#include "VertiCan_TX.h"
extern Adafruit_FlashTransport_SPI flashTransport;
extern Adafruit_SPIFlash flash;
extern FatFileSystem fatfs;
extern RH_RF69 rfm69;

//extern File dataFile = fatfs.open(FILE_NAME, FILE_READ);
#define FILE_NAME "data.csv"

bool confirmFormat(void);
char init_backup_management(void);
void formatMemory(void);
void extractData(void); 

void extractDataToBase(void);