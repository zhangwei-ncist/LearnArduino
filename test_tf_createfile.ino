/*
  【Arduino】168种传感器模块系列实验（资料代码+仿真编程+图形编程）
  程序六：Arduino在SD卡上创建和删除文件
  实验接线：
  Micro SD      UNO
  CS-------------10
  MOSI（DI）------11
  MISO（DO）------12
  CLK------------13
*/

#include <SPI.h>
#include <SD.h>
 
// 创建File实例化对象
File myFile;
 
void setup(){
    // 初始化硬件串口并设置波特率为9600
    Serial.begin(9600);
    
    while (!Serial)
    {
        ; //等待串口连接，仅使用本地 USB 端口
    }
 
    Serial.print("正在初始化 SD 卡...");
 
 
    // 检测是否初始化完成
    if (!SD.begin(4))
    {
        Serial.println("初始化失败！");
        return;
    }
    Serial.println("初始化完成。");
 
    // 查看是否存在"example.txt"文件
    if (SD.exists("example.txt"))
    {
        Serial.println("example.txt exists.");
    }
    else
    {
        Serial.println("example.txt 不存在");
    }
 
    // 打开一个新文件，然后立即将其关闭(创建文件)
    Serial.println("创建 example.txt...");
    myFile = SD.open("example.txt", FILE_WRITE);
    myFile.close();
 
    // 检查该文件是否新建成功
    if (SD.exists("example.txt"))
    {
        Serial.println("有example.txt文件了");
    }
    else
    {
        Serial.println("example.txt 不存在");
    }
 
    //删除该文件
    Serial.println("移除 example.txt...");
    SD.remove("example.txt");
 
    if (SD.exists("example.txt"))
    {
        Serial.println("example.txt exists.");
    }
    else
    {
        Serial.println("example.txt 不存在。");
    }
}
 
void loop(){
}
