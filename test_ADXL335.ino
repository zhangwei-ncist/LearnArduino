/*
  ADXL3xx

  Reads an Analog Devices ADXL3xx accelerometer and communicates the
  acceleration to the computer. The pins used are designed to be easily
  compatible with the breakout boards from SparkFun, available from:
  https://www.sparkfun.com/categories/80

  The circuit:
  - analog 0: accelerometer self test
  - analog 1: z-axis
  - analog 2: y-axis
  - analog 3: x-axis
  - analog 4: ground
  - analog 5: vcc

  created 2 Jul 2008
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/ADXL3xx
*/

// these constants describe the pins. They won't change:
// const int groundpin = 18;  // analog input pin 4 -- ground
// const int powerpin = 19;   // analog input pin 5 -- voltage
const int groundpin = A0;  // analog input pin 4 -- ground
const int powerpin = A4;   // analog input pin 5 -- voltage
const int xpin = A3;       // x-axis of the accelerometer
const int ypin = A2;       // y-axis
const int zpin = A1;       // z-axis (only on 3-axis models)

// initialize minimum and maximum Raw Ranges for each axis
int RawMin = 0;
int RawMax = 1023;
// Take multiple samples to reduce noise
const int sampleSize = 1;  //平均采样次数,感觉改成10次也变化不大

void setup() {
  // initialize the serial communications:
  Serial.begin(9600);
  //把模拟口当做数字口用，可以
  // Provide ground and power by using the analog inputs as normal digital pins.
  // This makes it possible to directly connect the breakout board to the
  // Arduino. If you use the normal 5V and GND pins on the Arduino,
  // you can remove these lines.
  pinMode(groundpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(groundpin, LOW);  //低电平0V
  digitalWrite(powerpin, HIGH);  //高电平5V
  analogReference(EXTERNAL);     //设置模拟参考地 3.3v，不设置误差大
}

void loop() {
  //Read raw values 原始数据  0-1023
  int xRaw = ReadAxis(xpin);
  int yRaw = ReadAxis(ypin);
  int zRaw = ReadAxis(zpin);

  // Convert raw values to 'milli-Gs" 映射到-3000 3000
  long xScaled = map(xRaw, RawMin, RawMax, -3000, 3000);
  long yScaled = map(yRaw, RawMin, RawMax, -3000, 3000);
  long zScaled = map(zRaw, RawMin, RawMax, -3000, 3000);

  // re-scale to fractional Gs
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;

  Serial.print("X, Y, Z  :: ");
  Serial.print(xRaw);
  Serial.print(", ");
  Serial.print(yRaw);
  Serial.print(", ");
  Serial.print(zRaw);
  Serial.print(" :: ");
  Serial.print(xAccel, 0);//
  Serial.print("G, ");
  Serial.print(yAccel, 0);
  Serial.print("G, ");
  Serial.print(zAccel, 0);//手里这个片，Z轴有问题。Z倒置式不灵敏
  Serial.println("G");
  delay(200);
}
// Take samples and return the average
int ReadAxis(int axisPin) {
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++) {
    reading += analogRead(axisPin);
  }
  return reading / sampleSize;
}