#include <ESP32Servo.h>

Servo testServo;
// 测试左后舵机填 14，左前舵机填 13，右后填 2，右前填 3
const int testPin = 14; 

void setup() {
  Serial.begin(115200);
  // 先写入停止默认值，再挂载引脚，避免上电乱转
  testServo.write(90);
  testServo.attach(testPin);
  Serial.println("=== 360°舵机零点校准工具 ===");
  Serial.println("输入 0-180 之间的数字，按回车发送");
  Serial.println("90 附近为停止点：数值越小正转越快，数值越大反转越快");
  Serial.println("请微调数值，找到舵机完全静止的数值并记录");
}

void loop() {
  if (Serial.available()) {
    // 读取整行数据，自动消耗换行符，避免残留
    String inputStr = Serial.readStringUntil('\n');
    inputStr.trim(); // 清除空格、回车等空白字符
    
    if (inputStr.length() > 0) {
      int val = inputStr.toInt();
      if (val >= 0 && val <= 180) {
        testServo.write(val);
        Serial.print("当前PWM值：");
        Serial.println(val);
      } else {
        Serial.println("请输入 0-180 之间的数值");
      }
    }
  }
}