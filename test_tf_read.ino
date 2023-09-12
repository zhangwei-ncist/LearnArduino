/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>

File myFile;
int chipSelect=4;
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
pinMode(chipSelect,OUTPUT);////如果不用默认的10引脚作为SS的话，记得加一句 pinMode(chipSelect,OUTPUT);否则容易初始化卡失败，就是选不到卡

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");




  // re-open the file for reading:
  myFile = SD.open("datalog.txt");
  if (myFile) {
    Serial.println("查看datalog.txt内容如下:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    Serial.print("读取文件结束!");
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening datalog.txt");
  }
}

void loop() {
  // nothing happens after setup
}


