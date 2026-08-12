# AI Car — 基于 ESP32-S3 的多模式 AI 语音控制移动机器人

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP32](https://img.shields.io/badge/platform-ESP32--S3-blue)](https://www.espressif.com/)
[![Arduino](https://img.shields.io/badge/Arduino-IDE-green)](https://www.arduino.cc/)

> 一个集固定语音指令、大模型自然语言理解和蓝牙遥控于一体的智能小车，支持姿态感知、自动避障和复合动作执行。

---

## 📖 项目简介

本项目基于 **ESP32-S3-DevKitC-1** 主控，融合了**三套控制模式**：

- **固定语音模式** — 通过天问语音模块（ASRPRO）发送预设指令，实现前进、后退、转向等基础动作。
- **AI 大模型语音模式** — 通过 INMP441 麦克风采集语音，连接豆包大模型 ASR + 推理模型，将自然语言（如“右转90度再前进20厘米”）解析为动作序列并执行。
- **蓝牙控制模式** — 通过手机 APP 发送摇杆或按键数据，实现实时遥控。

此外，小车搭载 **MPU6500 六轴姿态传感器**，实现精确的航向角闭环控制；**超声波模块**实时检测前方障碍物，自动紧急停止；**OLED 屏幕**显示当前模式、识别状态和实时距离。

项目旨在提供一个开源、可扩展的移动机器人平台，适用于教育、科研和 DIY 爱好者。

---

## ✨ 主要功能

| 功能模块 | 描述 |
|----------|------|
| **三模控制** | 按键（GPIO8）循环切换：固定语音 → AI 语音 → 蓝牙，状态实时显示在 OLED 上。 |
| **固定语音控制** | 通过 UART2 接收天问语音模块的指令码（0~7），执行预设动作（前进/后退/左/右/停止/坐下/趴下/跳舞）。 |
| **AI 大模型语音控制** | 支持自然语言指令，例如“正方形边长30厘米”、“右转45度后退10厘米”，自动解析为动作队列并执行。 |
| **复合动作队列** | 支持多段直行 + 转向组合，可执行正方形、三角形、五边形等几何轨迹。 |
| **精确转向控制** | 利用 MPU6500 的航向角（Yaw）进行闭环反馈，转向角度精度高（误差 < 2°）。 |
| **超声波自动避障** | 实时检测前方距离，小于安全距离（5cm）时立即紧急停止，清空所有动作队列。 |
| **OLED 状态显示** | 实时显示控制模式、当前识别状态、障碍物距离（厘米）。 |
| **蓝牙遥控（手机 APP）** | 支持“蓝牙串口”类 APP，发送 `[joystick, LV, RH]` 或 `[key, cmd]` 数据包进行控制。 |

---

## 🛠️ 硬件清单

| 组件 | 型号 / 规格 | 数量 | 备注 |
|------|-------------|------|------|
| 主控开发板 | ESP32-S3-DevKitC-1 | 1 | |
| 舵机 | SG90 360° 连续旋转舵机 | 4 | 四轮驱动 |
| 姿态传感器 | MPU6500 (I2C) | 1 | 替代 MPU6050，ID=0x70 |
| 超声波模块 | HC-SR04 或兼容 | 1 | 测距范围 2~400cm |
| 麦克风 | INMP441 (I2S) | 1 | 用于 AI 语音采集 |
| 语音模块 | 天问 ASRPRO 核心板 | 1 | 固定语音指令发送 |
| 蓝牙模块 | HC-04 (UART) | 1 | 与手机通信 |
| OLED 显示屏 | 0.96 寸 128×64 (SSD1306, I2C) | 1 | |
| 按键 | 轻触开关 (GPIO8) | 1 | 模式切换 |
| 电源 | 5V / 2A 稳压电源 | 1 | 为 ESP32 和舵机供电 |

---

## 📌 引脚连接说明

| 外设 | ESP32-S3 引脚 | 备注 |
|------|---------------|------|
| 舵机 1（右后） | GPIO2 | |
| 舵机 2（左后） | GPIO14 | |
| 舵机 3（右前） | GPIO13 | |
| 舵机 4（左前）| GPIO3 | |
| OLED I2C SDA | GPIO5 | |
| OLED I2C SCL | GPIO6 | |
| 蓝牙模块 RX (HC-04) | GPIO16 (UART1 TX) | 注意交叉连接 |
| 蓝牙模块 TX (HC-04) | GPIO15 (UART1 RX) | 注意交叉连接 |
| 天问语音模块 RX | GPIO17 (UART2 TX) | 接语音模块的 PA6 |
| 天问语音模块 TX | GPIO18 (UART2 RX) | 接语音模块的 PA5 |
| 超声波 TRIG | GPIO36 | |
| 超声波 ECHO | GPIO7 | |
| 麦克风 BCLK | GPIO10 | |
| 麦克风 WS | GPIO11 | |
| 麦克风 SD | GPIO12 | |
| 模式切换按键 | GPIO8 | 内部上拉，低电平有效 |

> **注意**：舵机需外接 5V 电源，ESP32-S3 的 3.3V 无法驱动。

---

## 🚀 快速开始

### 1. 环境准备

- 安装 [Arduino IDE](https://www.arduino.cc/en/software) 或 [PlatformIO](https://platformio.org/)。
- 在 Arduino IDE 中安装 **ESP32 开发板支持**（搜索 `esp32`，版本 ≥ 2.0.0）。
- 安装以下第三方库（可通过库管理器或手动下载）：
  - `ESP32Servo` — 舵机控制
  - `Adafruit GFX` + `Adafruit SSD1306` — OLED 显示
  - `WebSockets` — WebSocket 通信
  - `ArduinoJson` — JSON 解析
  - `HTTPClient` — HTTP 请求（ESP32 自带）

### 2. 配置 WiFi 和 API 密钥

在 `AI_car.h` 中修改以下宏定义：

```cpp
// WiFi 名称和密码
const char* WIFI_SSID     = "你的 WiFi 名称";
const char* WIFI_PWD      = "你的 WiFi 密码";

// 豆包 ASR 语音识别 API Key 和资源 ID
const char* DOUBAO_ASR_API_KEY  = "你的 ASR API_Key";
const char* ASR_RESOURCE_ID     = "volc.bigasr.sauc.duration";

// 豆包大模型推理 API Key 和接入点 ID（在 parseLLMToActions 函数中）
// 需修改以下两行：
http.addHeader("Authorization", "Bearer 你的推理模型 API_Key");
reqDoc["model"] = "你的推理模型接入点 ID";

