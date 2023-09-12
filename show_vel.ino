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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
}

void loop() {
  // put your main code here, to run repeatedly:
showVel(12345,20000);//显示速度信息
  delay(2000);
}
void  showVel(int l, int r)
{
  display.clearDisplay();
  drawrect();  //画个边框
  drawInfo(l,r);
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
  display.setCursor(display.width() / 4, 2);
  display.print(F("L "));
  display.setCursor(display.width() * 3 / 4, 2);
  display.print(F("R"));

//写数字
  display.setCursor(3, 12);
  display.setTextSize(2);  //具体数字放大
  display.print(l);

  display.setCursor(67, 12);
  display.setTextSize(2);  //具体数字放大
  display.print(r);
}