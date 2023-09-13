/*测试
1.L298N驱动左右电机
2.左右电机的AB相编码器是否准确*/
/*
L298N模块连接 =uno 引脚
*/
int ENA = 9;
int IN1 = 8;
int IN2 = 7;

int ENB = 10;
int IN3 = 6;
int IN4 = 5;

/*
电机霍尔传感器连接 uno
*/
int right_motor_c1 = 2;  // int0
int right_motor_c2 = 12;
int left_motor_c1 =3;  // int1
int left_motor_c2 = 13;

/*一个定义为volatile的变量就是说这个变量可能会被意想不到的改变，这样，编译器就不会去随便假设这个变量的值了。精确的说，优化器在用到这个变量的值的时候，必须每次都小心的重新读取这个变量的值，而不是使用保存在寄存器里面的备份。

下面是volatile变量的几个例子：

1）. 并行设备的硬件寄存器（如：状态寄存器）

2）. 一个中断服务子程序中会访问到的非自动变量（Non-automatic variables)

3）. 多线程应用中被几个任务共享的变量*/

volatile int countLeft = 0;   //如果是正转，那么每计数一次自增1，如果是反转，那么每计数一次自减1
volatile int countRight = 0;  //如果是正转，那么每计数一次自增1，如果是反转，那么每计数一次自减1

int reducation = 90;                     //减速比，根据电机参数设置，比如 15 | 30 | 60
int pulse = 11;                          //编码器旋转一圈产生的脉冲数该值需要参考商家电机参数
int per_round = pulse * reducation * 2;  //车轮旋转一圈产生的脉冲数
///手动旋转电机一圈，输出结果为 一圈脉冲数 * 减速比 * 2
//UNO R3只有两个外部中断，所以，左右轮各一个。只能进行2倍频统计

long start_time = millis();  //一个计算周期的开始时刻，初始值为 millis();
long interval_time = 50;     //一个计算周期 50ms
double current_vel;

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
  //2倍频计数实现
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


void setup() {
  // put your setup code here, to run once:

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


  Serial.begin(57600);

  SetMotorDirection('a', 'f');
  SetMotorDirection('b', 'f');


  attachInterrupt(0, count_right, CHANGE);  //当电平发生改变时触发中断0函数
  attachInterrupt(1, count_left, CHANGE);   //当电平发生改变时触发中断1函数.
Serial.println( "测试1：请观察电机是否转动！左1s，右1s");
  analogWrite(ENA, 200);  //可输入的数字为0-255当然电机低于某个电压就会不转了，大家可以更改数
  delay(1000);
  analogWrite(ENA, 0);

  analogWrite(ENB, 200);
  delay(1000);
  analogWrite(ENB, 0);
Serial.println( "测试2：输出左右计数器，它们不应相差太多！");
  //测试计数器输出
  delay(2000);
  Serial.println( countLeft);
  Serial.println( countRight);
countLeft=0;
countRight=0;
delay(10);
Serial.print("测试3：请手动旋转左右轮胎各一圈，脉冲计数应为");
Serial.println(per_round);

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

void loop() {
  // put your main code here, to run repeatedly:
  delay(5);
  Serial.print("L=");
  Serial.println( countLeft);
    Serial.print("R=");
  Serial.println( countRight);


}
