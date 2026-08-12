#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED 配置和你原工程保持一致
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define OLED_ADDR       0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 按键引脚定义
#define KEY_PIN 9
// 按键消抖延时ms
#define KEY_DELAY_MS 20
uint32_t lastKeyTick = 0;
uint8_t lastKeyState = HIGH;

void setup() {
  // 调试串口
  Serial.begin(115200);
  Serial.println("==== GPIO9按键独立测试程序 ====");

  // I2C初始化OLED SDA=5 SCL=6 和原代码匹配
  Wire.begin(5, 6);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED初始化失败，请检查硬件接线！");
    while (1); // OLED异常卡死提示
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Key Test GPIO9");
  display.println("Status: IDLE");
  display.display();

  // 按键初始化：内部上拉，按键按下为低电平
  pinMode(KEY_PIN, INPUT_PULLUP);
  lastKeyState = digitalRead(KEY_PIN);
}

void loop() {
  uint32_t now = millis();
  uint8_t currentKey = digitalRead(KEY_PIN);

  // 消抖判断：间隔大于消抖延时才判定有效电平变化
  if ((now - lastKeyTick) > KEY_DELAY_MS) {
    if (currentKey != lastKeyState) {
      lastKeyTick = now;
      lastKeyState = currentKey;

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Key Test GPIO9");

      if (currentKey == LOW) {
        // 按键按下
        Serial.println("[KEY] GPIO9 按键按下");
        display.println("Status: PRESS");
      } else {
        // 按键松开
        Serial.println("[KEY] GPIO9 按键松开");
        display.println("Status: RELEASE");
      }
      display.display();
    }
  }

  delay(5);
}

