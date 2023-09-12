/*
 * 控制LED灯开关，在一个循环周期内前两秒使LED灯处于点亮状态，后两秒关闭LED灯
 * 1.setup 中设置引脚为输出模式
 * 2.loop 中向引脚输出高电压，休眠 2000 毫秒后，再输出低电压，再休眠 2000 毫秒
 * 
 */
// int led = 3;
int r_led = 6;
int g_led = 5;
int b_led = 3;
void setup() {
  // Serial.begin(57600);
  pinMode(r_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  pinMode(b_led, OUTPUT);
}
/*利用pwm给led调个颜色*/
void set_led_pwm(int led,int value) {
  analogWrite(led, value);
}
void setRGB(int r,int g,int b){//设置
  set_led_pwm(r_led,r);
  set_led_pwm(g_led,g);
  set_led_pwm(b_led,b);
}
void resetRGB(){//清除
    set_led_pwm(r_led,0);
  set_led_pwm(g_led,0);
  set_led_pwm(b_led,0);
}
void loop() {
  set_led_pwm(r_led,100);
  delay(1000);
    set_led_pwm(r_led,0);

    set_led_pwm(g_led,100);
  delay(1000);
      set_led_pwm(g_led,0);

    set_led_pwm(b_led,100);
  delay(1000);
      set_led_pwm(b_led,0);
  delay(1000);
    setRGB(200,100,200);
      delay(2000);
      resetRGB();
}
