/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;//如果不用默认的10引脚作为SS的话，记得加一句 pinMode(chipSelect,OUTPUT);否则容易初始化卡失败，就是选不到卡

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

pinMode(chipSelect,OUTPUT);////如果不用默认的10引脚作为SS的话，记得加一句 pinMode(chipSelect,OUTPUT);否则容易初始化卡失败，就是选不到卡
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");

  // 查看是否存在"example.txt"文件
  if (SD.exists("datalog.txt")) {
    Serial.println("datalog.txt exists.");
  } else {
    Serial.println("datalog.txt 不存在");
    // 打开一个新文件，然后立即将其关闭(创建文件)
    Serial.println("创建 datalog.txt...");
    File myFile = SD.open("datalog.txt", FILE_WRITE);
    myFile.close();
    Serial.println("创建 datalog.txt完成！");
  }
}

void loop() {
  delay(2000);  //2s写一次
  // make a string for assembling the data to log:
  String dataString = "Runing...";

  // read three sensors and append to the string:
  // for (int analogPin = 0; analogPin < 3; analogPin++) {
  //   int sensor = analogRead(analogPin);
  //   dataString += String(sensor);
  //   if (analogPin < 2) {
  //     dataString += ",";
  //   }
  // }
  dataString += (millis());

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);

    // print to the serial port too:
    Serial.println(dataString);
    dataFile.close();
  } else {  // if the file isn't open, pop up an error:
    // Serial.println("error opening datalog.txt");
    Serial.println("datalog.txt open error!");
  }
}
