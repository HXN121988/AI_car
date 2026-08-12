#include <ESP32Servo.h>

// ========== 和你原工程完全一致硬件参数 ==========
const int servoPinFrontLeft  = 2;
const int servoPinFrontRight = 14;
const int servoPinBackLeft   = 3;
const int servoPinBackRight  = 13;

// 修复：正确定义四个舵机对象
Servo servoFL;
Servo servoFR;
Servo servoBL;
Servo servoBR;

// 舵机速度标定（和原代码统一）
const int STOP_SPEED    = 95;
const int LEFT_FORWARD  = 90;
const int LEFT_BACK     = 100;
const int RIGHT_FORWARD = 100;
const int RIGHT_BACK    = 80;

// 标定输出变量
float calStraightMsPerCm = 0.0f;
float calTurnMsPerDeg    = 0.0f;

// ========== 函数前置声明（解决未定义报错） ==========
void stopAll();
void runForward();
void runTurnRight();
void runTurnLeft();
uint32_t testStraight(int targetCm);
uint32_t testTurn(int targetDeg, int dir);
void serialCmdParse(String cmd);

// ========== 运动函数 ==========
void stopAll() {
  servoFL.write(STOP_SPEED);
  servoFR.write(STOP_SPEED);
  servoBL.write(STOP_SPEED);
  servoBR.write(STOP_SPEED);
}

// 四轮同步直行
void runForward() {
  servoFL.write(LEFT_FORWARD);
  servoBL.write(LEFT_FORWARD);
  servoFR.write(RIGHT_FORWARD);
  servoBR.write(RIGHT_FORWARD);
}

// 原地右转（你代码6号动作）
void runTurnRight() {
  servoFL.write(LEFT_FORWARD);
  servoBL.write(LEFT_FORWARD);
  servoFR.write(RIGHT_BACK);
  servoBR.write(RIGHT_BACK);
}

// 原地左转（你代码5号动作）
void runTurnLeft() {
  servoFL.write(LEFT_BACK);
  servoBL.write(LEFT_BACK);
  servoFR.write(RIGHT_FORWARD);
  servoBR.write(RIGHT_FORWARD);
}

// ========== 标定测试函数 ==========
// 测试直行距离：cm=目标厘米，返回总耗时ms
uint32_t testStraight(int targetCm) {
  Serial.printf("===== 开始直行 %d 厘米测试 =====\n", targetCm);
  uint32_t tStart = millis();
  runForward();
  while(1) {
    delay(10);
    // 串口输入任意字符停止
    if(Serial.available()) {
      Serial.read();
      stopAll();
      uint32_t totalMs = millis() - tStart;
      calStraightMsPerCm = (float)totalMs / targetCm;
      Serial.printf("直行总耗时：%d ms\n", totalMs);
      Serial.printf("标定结果 STRAIGHT_MS_PER_CM = %.2f\n\n", calStraightMsPerCm);
      return totalMs;
    }
  }
}

// 测试原地转向：deg目标角度，dir 1右转 / -1左转
uint32_t testTurn(int targetDeg, int dir) {
  if(dir == 1) Serial.printf("===== 开始原地右转 %d° 测试 =====\n", targetDeg);
  else Serial.printf("===== 开始原地左转 %d° 测试 =====\n", targetDeg);

  uint32_t tStart = millis();
  if(dir == 1) runTurnRight();
  else runTurnLeft();

  while(1) {
    delay(10);
    if(Serial.available()) {
      Serial.read();
      stopAll();
      uint32_t totalMs = millis() - tStart;
      calTurnMsPerDeg = (float)totalMs / targetDeg;
      Serial.printf("转向总耗时：%d ms\n", totalMs);
      Serial.printf("标定结果 TURN_MS_PER_DEGREE = %.2f\n\n", calTurnMsPerDeg);
      return totalMs;
    }
  }
}

// ========== 串口指令解析 ==========
void serialCmdParse(String cmd) {
  cmd.trim();
  // F40直行40cm / R90右转90度 / L90左转90度 / S停止
  if(cmd[0] == 'F' || cmd[0] == 'f') {
    int cm = cmd.substring(1).toInt();
    if(cm > 0) testStraight(cm);
    else Serial.println("参数错误，示例 F30");
  }
  else if(cmd[0] == 'R' || cmd[0] == 'r') {
    int deg = cmd.substring(1).toInt();
    if(deg > 0) testTurn(deg, 1);
    else Serial.println("参数错误，示例 R90");
  }
  else if(cmd[0] == 'L' || cmd[0] == 'l') {
    int deg = cmd.substring(1).toInt();
    if(deg > 0) testTurn(deg, -1);
    else Serial.println("参数错误，示例 L90");
  }
  else if(cmd[0] == 'S' || cmd[0] == 's') {
    stopAll();
    Serial.println("小车已停止");
  }
  else {
    Serial.println("=== 指令说明 ===");
    Serial.println("Fxx 直行xx厘米  例:F40");
    Serial.println("Rxx 右转xx度    例:R90");
    Serial.println("Lxx 左转xx度    例:L90");
    Serial.println("S   紧急停止");
  }
}

void setup() {
  Serial.begin(115200);
  // 上电先停止，防止乱跑
  stopAll();
  servoFL.attach(servoPinFrontLeft);
  servoFR.attach(servoPinFrontRight);
  servoBL.attach(servoPinBackLeft);
  servoBR.attach(servoPinBackRight);

  delay(800);
  Serial.println("==== 四轮舵机标定工具就绪 ====");
  Serial.println("输入指令回车执行，走到标准刻度发任意字符停止");
  Serial.println();
}

void loop() {
  if(Serial.available()) {
    String buf = Serial.readStringUntil('\n');
    serialCmdParse(buf);
  }
  delay(20);
}
