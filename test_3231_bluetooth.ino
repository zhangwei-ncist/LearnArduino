 /*

【Arduino】168种传感器模块系列实验（资料代码+仿真编程+图形编程）

实验一百一十： JDY-31蓝牙转串口适配器SPP-C模块组无线透传 替换HC-05/06从机

项目五：最简单的蓝牙控制LED的程序，输入1为亮，其他为熄灭，串口显示输入字符

（手机端-应用宝-搜索《SPP》-安装-打开-搜蓝牙<JDY-31-SPP>-连接-设置控制面板）
发送数据用us-ascii模式
连接方式 LED D13

JDY-31模块与Uno：

VCC        VCC

GND        GND

TXD        D0

RXD        D1

*/



void setup(){

pinMode(13,OUTPUT);

Serial.begin(9600);

}



void loop(){

  if(Serial.available())

  {

  char num = Serial.read();

  Serial.println(num);

  if(num == '1')

  digitalWrite(13,HIGH);

  else

  digitalWrite(13,LOW);

  }

}