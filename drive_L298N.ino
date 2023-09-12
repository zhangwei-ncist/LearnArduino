
/*1306显示需要的*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10  // Number of snowflakes in the animation example
/*为了显示车速定义了一些显示用函数*/
void showVel(int l, int r) {
  display.clearDisplay();
  drawrect();  //画个边框
  drawInfo(l, r);
  display.display();  // Update screen with each newly-drawn rectangle
}
void drawrect(void) {

  display.drawRect(0, 0, display.width() / 2, display.height(), SSD1306_WHITE);
  display.drawRect(display.width() / 2, 0, display.width() / 2, display.height(), SSD1306_WHITE);
}
void drawInfo(int l, int r) {
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  //写标题
  display.setCursor(display.width() / 8, 2);
  display.print(F("L (rpm)"));
  display.setCursor(display.width() * 5 / 8, 2);
  display.print(F("R (rpm)"));

  //写数字
  display.setCursor(22, 14);
  display.setTextSize(2);  //具体数字放大
  display.print(l);

  display.setCursor(86, 14);
  display.setTextSize(2);  //具体数字放大
  display.print(r);
}

/*
L298N模块连接 =uno 引脚
*/
int ENA = 9;
int IN1 = 8;
int IN2 = 7;

int ENB = 4;
int IN3 = 6;
int IN4 = 5;

/*
电机型号：JGB-520
DC6V55RPM    11plus/电机圈   减速机构减速比90
电机霍尔传感器连接 uno
统计出来的count若为负，则可切换相线序号
*/
int right_motor_c1 = 12;
int right_motor_c2 = 2;  // int0
int left_motor_c1 = 3;   // int1
int left_motor_c2 = 13;

/*一个定义为volatile的变量就是说这个变量可能会被意想不到的改变，这样，编译器就不会去随便假设这个变量的值了。精确的说，优化器在用到这个变量的值的时候，必须每次都小心的重新读取这个变量的值，而不是使用保存在寄存器里面的备份。

下面是volatile变量的几个例子：

1）. 并行设备的硬件寄存器（如：状态寄存器）

2）. 一个中断服务子程序中会访问到的非自动变量（Non-automatic variables)

3）. 多线程应用中被几个任务共享的变量*/

volatile int countLeft = 0;   //如果是正转，那么每计数一次自增1，如果是反转，那么每计数一次自减1
volatile int countRight = 0;  //如果是正转，那么每计数一次自增1，如果是反转，那么每计数一次自减1

int reducation = 90;                           //减速比，根据电机参数设置，比如 15 | 30 | 60
int pulse = 11;                                //编码器旋转一圈产生的脉冲数该值需要参考商家电机参数
int wheel_per_round = pulse * reducation * 2;  //车轮旋转一圈产生的脉冲数，我这里两倍频
///手动旋转电机一圈，输出结果为 一圈脉冲数 * 减速比 * 2
//UNO R3只有两个外部中断，所以，左右轮各一个。只能进行2倍频统计

long start_time = millis();  //一个计算周期的开始时刻，初始值为 millis();
long interval_time = 50;     //一个计算周期 50ms
double left_wheel_current_vel, right_wheel_current_vel;

//左侧车轮中断函数，
//left_motor_c1 = 3;  // int1
// left_motor_c2 = 13;
void count_left() {
  //2倍频计数实现
  //手动旋转电机一圈，输出结果为 一圈脉冲数 * 减速比 * 2
  if (digitalRead(left_motor_c1) == HIGH) {

    if (digitalRead(left_motor_c2) == HIGH) {  //A 高 B 高
      countLeft++;
    } else {  //A 高 B 低
      countLeft--;
    }
  } else {

    if (digitalRead(left_motor_c2) == LOW) {  //A 低 B 低
      countLeft++;
    } else {  //A 低 B 高
      countLeft--;
    }
  }
}
//右侧车轮中断函数，
// right_motor_c1 = 2;  // int0
// right_motor_c2 = 12;
void count_right() {
  //2倍频计数实现，一个脉冲周期计数为2，而不是1
  //手动旋转电机一圈，输出结果为 一圈脉冲数 * 减速比 * 2
  if (digitalRead(right_motor_c1) == HIGH) {

    if (digitalRead(right_motor_c2) == HIGH) {  //A 高 B 高
      countRight++;
    } else {  //A 高 B 低
      countRight--;
    }
  } else {

    if (digitalRead(right_motor_c2) == LOW) {  //A 低 B 低
      countRight++;
    } else {  //A 低 B 高
      countRight--;
    }
  }
}


/*
思路:需要定义一个开始时间(用于记录每个测速周期的开始时刻)，还需要定义一个时间区间(比如50毫秒)，时时获取当前时刻，
当当前时刻 - 上传结束时刻 >= 时间区间时，就获取当前计数并根据测速公式计算时时速度，计算完毕，计数器归零，重置开始时间

核心知识点:当使用中断函数中的变量时，需要先禁止中断noInterrupts()，调用完毕，再重启中断interrupts()(关于noInterrupts与interrupts请参考 8.2.2 介绍)。
*/

//获取当前转速的函数
void get_current_vel() {
  long right_now = millis();
  long past_time = right_now - start_time;  //计算逝去的时间
  if (past_time >= interval_time) {         //如果逝去时间大于等于一个计算周期
    //1.禁止中断
    noInterrupts();
    //2.计算转速 转速单位可以是秒，也可以是分钟... 自定义即可
    left_wheel_current_vel = ((double)countLeft / wheel_per_round / past_time) * 1000 * 60;
    right_wheel_current_vel = ((double)countRight / wheel_per_round / past_time) * 1000 * 60;
    //3.重置计数器
    countLeft = 0;
    countRight = 0;
    //4.重置开始时间
    start_time = right_now;
    //5.重启中断
    interrupts();
    // Serial.print("LEFT WHEEL(RPM)=");
    // Serial.println(left_wheel_current_vel);
    // Serial.print("RIGHT WHEEL(RPM)=");
    // Serial.println(right_wheel_current_vel);
    showVel(int(left_wheel_current_vel), int(right_wheel_current_vel));  //显示速度信息
  }
}

/*
 * PID 调速实现:
 * 1.代码准备，复制并修改电机控制以及测速代码
 * 2.包含PID头文件
 * 3.创建PID对象
 * 4.在setup中启用自动调试
 * 5.调试并更新PWM
 * 
 */

#include <PID_v1.h>
//-------------------------------------PID-------------------------------------------
//创建 PID 对象
//1.当前转速 2.计算输出的pwm 3.目标转速 4.kp 5.ki 6.kd 7.当输入与目标值出现偏差时，向哪个方向控制
double left_pwm, right_pwm;         //电机驱动的PWM值
double target = 20;                 //预期的结果
double kp = 5, ki = 2.8, kd = 0.3;  //+5-5
/*
PID调试技巧:

参数整定找最佳，从小到大顺序查

先是比例后积分，最后再专把微分加

曲线振属荡很频繁，比例度盘要放大

曲线漂浮绕大湾，比例度盘往小扳

曲线偏离回复慢，积分时间往下降

曲线波动周期长，积分时间再加长

曲线振荡频率快，先把微分降下来**

动差大来波动慢。微分时间应加长

理想曲线两个波，前高后低4比1

一看二调多分析，调节质量不会低
*/
PID left_pid(&left_wheel_current_vel, &left_pwm, &target, kp, ki, kd, DIRECT);
PID right_pid(&right_wheel_current_vel, &right_pwm, &target, kp, ki, kd, DIRECT);
//速度更新函数
void update_vel() {
  //获取当前速度
  get_current_vel();
  left_pid.Compute();   //计算需要输出的PWM
  right_pid.Compute();  //计算需要输出的PWM
  // digitalWrite(DIRA,HIGH);
  analogWrite(ENA, left_pwm);
  analogWrite(ENB, right_pwm);
}


/*设置点击的前进，后退，停止*/
void SetMotorDirection(char motor, char direction) {  //motor= 'a' or 'b'  direction= 'f' or 'b' or 's'
  int motor_inOne, motor_inTwo;
  if (motor == 'a') {  //设置L298N的引脚
    motor_inOne = IN1;
    motor_inTwo = IN2;
  } else if (motor == 'b') {
    motor_inOne = IN3;
    motor_inTwo = IN4;
  } else {
    Serial.println("SetMotorDirection motor Error!");
  }
  if (direction == 'f') {
    digitalWrite(motor_inOne, LOW);  //正转
    digitalWrite(motor_inTwo, HIGH);
  } else if (direction == 'b') {
    digitalWrite(motor_inOne, HIGH);  //反转
    digitalWrite(motor_inTwo, LOW);
  } else if (direction == 's') {
    digitalWrite(motor_inOne, LOW);  //刹车
    digitalWrite(motor_inTwo, LOW);
  } else {
    Serial.println("SetMotorDirection directiron Error!");
  }
}

void setup() {
  // put your setup code here, to run once:
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }


  /*设置L298N的输出引脚*/
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  /*设置到霍尔传感器的输入引脚*/
  pinMode(right_motor_c1, INPUT);
  pinMode(right_motor_c2, INPUT);
  pinMode(left_motor_c1, INPUT);
  pinMode(left_motor_c2, INPUT);

  attachInterrupt(0, count_right, CHANGE);  //当电平发生改变时触发中断0函数
  attachInterrupt(1, count_left, CHANGE);   //当电平发生改变时触发中断1函数

  Serial.begin(57600);

  SetMotorDirection('a', 'f');
  SetMotorDirection('b', 'f');

  left_pid.SetMode(AUTOMATIC);
  right_pid.SetMode(AUTOMATIC);
  // analogWrite(ENA, 250);  //可输入的数字为0-255当然电机低于某个电压就会不转了，大家可以更改数
  // delay(10000);
  // analogWrite(ENA, 0);

  // analogWrite(ENB, 150);
  // delay(10000);
  // analogWrite(ENB, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  //测试计数器输出
  // delay(100);
  // // Serial.println(countLeft);
  // // Serial.println(countRight);
  // get_current_vel();

  delay(50);
  update_vel();
}
