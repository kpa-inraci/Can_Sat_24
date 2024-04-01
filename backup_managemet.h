#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include "ff.h"
#include "diskio.h"

extern Adafruit_FlashTransport_SPI flashTransport;
extern Adafruit_SPIFlash flash;
extern FatFileSystem fatfs;


#define FILE_NAME      "data.csv"

bool confirmFormat(void);
void init_backup_management(void);
void formatMemory(void);
void extractData(void); 
char backup_choice(void);