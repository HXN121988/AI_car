<div align="center">

# 🤖 AI Car — 基于 ESP32-S3 的多模式 AI 语音控制移动机器人

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: ESP32](https://img.shields.io/badge/platform-ESP32--S3-blue)](https://www.espressif.com/)
[![Arduino](https://img.shields.io/badge/Arduino-IDE-green)](https://www.arduino.cc/)
[![Framework](https://img.shields.io/badge/Framework-ArduinoCore--ESP32-orange)](https://github.com/espressif/arduino-esp32)

**一个集固定语音指令、大模型自然语言理解和蓝牙遥控于一体的智能小车，支持姿态感知、自动避障和复合动作执行。**

[快速开始](#-快速开始) · [使用说明](#-使用说明) · [通信协议](#-通信协议) · [系统架构](#️-系统架构与工作原理) · [常见问题](#-常见问题-faq)

</div>

---

## 📖 项目简介

本项目基于 **ESP32-S3-DevKitC-1** 主控，融合了 **三套控制模式**，一键切换：

- 🎙️ **固定语音模式** — 通过天问语音模块（ASRPRO）发送预设指令，实现前进、后退、转向等基础动作，离线可用。
- 🧠 **AI 大模型语音模式** — 通过 INMP441 麦克风采集语音，连接豆包大模型 ASR + 推理模型，将自然语言（如"右转90度再前进20厘米"）解析为动作序列并执行。
- 📱 **蓝牙控制模式** — 通过手机 APP 发送摇杆或按键数据，实现实时遥控。

此外，小车搭载 **MPU6500 六轴姿态传感器**，实现精确的航向角闭环控制；**超声波模块**实时检测前方障碍物，自动紧急停止；**OLED 屏幕**显示当前模式、识别状态和实时距离。

> 项目旨在提供一个开源、可扩展的移动机器人平台，适用于教育、科研和 DIY 爱好者。

---

## ✨ 主要功能

| 功能模块 | 描述 |
|----------|------|
| 🔄 **三模控制** | 按键（GPIO8）循环切换：固定语音 → AI 语音 → 蓝牙，状态实时显示在 OLED 上。 |
| 🎙️ **固定语音控制** | 通过 UART2 接收天问语音模块的指令码（0~7），执行预设动作。 |
| 🧠 **AI 大模型语音控制** | 支持自然语言指令，例如"正方形边长30厘米"、"右转45度后退10厘米"，自动解析为动作队列并执行。 |
| 📋 **复合动作队列** | 支持多段直行 + 转向组合，可执行正方形、三角形、五边形等几何轨迹，最多 20 条动作。 |
| 🎯 **精确转向控制** | 利用 MPU6500 的航向角（Yaw）进行闭环反馈，分级调速，转向角度误差 < 2°。 |
| 🛑 **超声波自动避障** | 实时检测前方距离，小于安全距离（5cm）时立即紧急停止，清空所有动作队列，全模式生效。 |
| 📺 **OLED 状态显示** | 实时显示控制模式、当前识别状态、障碍物距离（厘米）。 |
| 📱 **蓝牙遥控** | 支持"蓝牙串口"类 APP，发送 `[joystick, LV, RH]` 或 `[key, cmd]` 数据包进行控制。 |
| ⚡ **全非阻塞架构** | 无长 `delay()`，超声波、陀螺仪、语音、蓝牙并行运行，响应流畅。 |

---

## 🛠️ 硬件清单

| 组件 | 型号 / 规格 | 数量 | 备注 |
|------|-------------|------|------|
| 主控开发板 | ESP32-S3-DevKitC-1 | 1 | 推荐 N16R8 版本 |
| 舵机 | SG90 360° 连续旋转舵机 | 4 | 四轮驱动 |
| 姿态传感器 | MPU6500 (I2C) | 1 | 芯片 ID = 0x70 |
| 超声波模块 | HC-SR04 或兼容 | 1 | 测距范围 2~400cm |
| 麦克风 | INMP441 (I2S) | 1 | 用于 AI 语音采集 |
| 语音模块 | 天问 ASRPRO 核心板 | 1 | 固定语音指令 + 唤醒 |
| 蓝牙模块 | HC-04 (UART) | 1 | 与手机通信，波特率 9600 |
| OLED 显示屏 | 0.96 寸 128×64 (SSD1306, I2C) | 1 | 地址 0x3C |
| 按键 | 轻触开关 (GPIO8) | 1 | 模式切换，内部上拉 |
| 电源 | 5V / 2A 稳压电源 | 1 | 为 ESP32 和舵机供电 |

> ⚠️ **注意**：舵机需外接 5V 独立电源，ESP32-S3 的 3.3V 无法驱动；所有模块 GND 必须共地。

---

## 📌 引脚连接说明

| 外设 | ESP32-S3 引脚 | 备注 |
|------|---------------|------|
| 舵机 1（左前） | GPIO2 | Front Left |
| 舵机 2（右前） | GPIO14 | Front Right |
| 舵机 3（左后） | GPIO3 | Back Left |
| 舵机 4（右后） | GPIO13 | Back Right |
| OLED I2C SDA | GPIO5 | 与 MPU6500 共用 I2C 总线 |
| OLED I2C SCL | GPIO6 | 与 MPU6500 共用 I2C 总线 |
| 蓝牙模块 RX (HC-04) | GPIO15 (UART1 TX) | 交叉连接 |
| 蓝牙模块 TX (HC-04) | GPIO16 (UART1 RX) | 交叉连接 |
| 天问语音模块 RX | GPIO17 (UART2 TX) | 接语音模块 PA6 |
| 天问语音模块 TX | GPIO18 (UART2 RX) | 接语音模块 PA5 |
| 超声波 TRIG | GPIO36 | |
| 超声波 ECHO | GPIO7 | |
| 麦克风 BCLK | GPIO10 | |
| 麦克风 WS | GPIO11 | |
| 麦克风 SD | GPIO12 | |
| 模式切换按键 | GPIO8 | 内部上拉，低电平有效 |

---

## 🚀 快速开始

### 1. 环境准备

- 安装 [Arduino IDE](https://www.arduino.cc/en/software) 或 [PlatformIO](https://platformio.org/)。
- 在 Arduino IDE 中安装 **ESP32 开发板支持**（搜索 `esp32`，版本 ≥ 2.0.0）。
- 安装以下第三方库（库管理器搜索安装）：

| 库名 | 用途 |
|------|------|
| `ESP32Servo` | 舵机控制 |
| `Adafruit GFX Library` | OLED 图形基础 |
| `Adafruit SSD1306` | OLED 屏幕驱动 |
| `WebSockets` | WebSocket 通信（Links2004 版本） |
| `ArduinoJson` | JSON 解析（版本 6.x） |
| `HTTPClient` | HTTP 请求（ESP32 自带，无需安装） |

### 2. 配置 WiFi 和 API 密钥

在 `AI_car.h` 中修改以下配置：

```cpp
// ==================== WiFi 配置 自行修改 ====================
const char* WIFI_SSID     = "你的 WiFi 名称";
const char* WIFI_PWD      = "你的 WiFi 密码";

// ==================== 豆包 ASR 流式语音识别配置 ====================
const char* DOUBAO_ASR_API_KEY  = "你的 ASR API_Key";
const char* ASR_RESOURCE_ID     = "volc.bigasr.sauc.duration"; // 官方固定值，无需修改
```

在 `AI_car.cpp` 的 `parseLLMToActions()` 函数中修改大模型配置：

```cpp
http.addHeader("Authorization", "Bearer 你的推理模型 API_Key");
reqDoc["model"] = "你的推理模型接入点 ID"; // ep- 开头
```

<details>
<summary>🔑 密钥获取方式（点击展开）</summary>

- **ASR API Key**：登录 [火山引擎控制台](https://console.volcengine.com/speech/) → 语音识别 → 开通「流式语音识别大模型1.0」→ API Key 管理 → 创建密钥。
- **推理模型 API Key / 接入点 ID**：登录 [火山方舟控制台](https://console.volcengine.com/ark/) → 模型推理 → 创建接入点（推荐 Doubao 系列模型）→ 获取 API Key 和接入点 ID（`ep-` 开头）。
- 两个服务均需完成实名认证后开通。

</details>

### 3. 编译与上传

1. 将 `AI_car.h` 和 `AI_car.cpp` 放入同一文件夹（文件夹名建议 `AI_car`）。
2. 在 Arduino IDE 中打开主文件。
3. **开发板选择**：`Tools → Board → ESP32 Arduino → ESP32S3 Dev Module`。
4. **分区表选择**：`Tools → Partition Scheme → Default 4MB with spiffs`。
5. 连接 ESP32-S3 到电脑，选择对应串口，点击 **上传**。
6. 上传完成后打开串口监视器（波特率 `115200`），观察初始化日志。

### 4. 首次运行与校准

1. **上电前**：确保小车放置在水平地面，上电后 **不要移动小车**，陀螺仪会自动执行零点校准（约 1 秒）。
2. 串口监视器应依次输出：

```
四轮语音机器人控制程序启动
[MPU] MPU6500初始化成功，ID=0x70
[MPU] 开始陀螺仪校准，请保持小车静止...
[MPU] 校准完成，Z轴零偏: x.xx
WiFi连接成功
INMP441 麦克风初始化完成
GPIO8按键初始化完成，默认固定语音模式
初始化完成，等待指令
```

3. OLED 屏幕显示当前模式（默认 `Fix Voice`）和前方距离。
4. 若 MPU6500 初始化失败，程序会自动降级为纯时间控制转向，不影响基础运行。

---

## 🎮 使用说明

### 模式切换

按下 **GPIO8 按键** 循环切换三种模式，切换后有 2 秒冷却防误触，OLED 实时显示当前模式：

```
固定语音 (Fix Voice) → AI大模型 (AI Voice) → 蓝牙 (Bluetooth) → 循环
```

### 模式一：固定语音模式（默认）

- 通过 **天问 ASRPRO 语音模块** 识别预设关键词，模块通过 UART2（115200 波特率）发送单字节指令。
- 直接说对应关键词即可控制小车，**无需联网**。
- 指令编码见 [通信协议](#-通信协议)。

### 模式二：AI 大模型语音模式

1. 切换到 `AI Voice` 模式后，小车处于空闲等待状态。
2. 对天问语音模块说 **唤醒词**（需在 ASRPRO 中配置为发送 `0xFF`），触发云端语音识别。
3. OLED 显示 `Connecting...` → `Speak now`，此时对 INMP441 麦克风说话。
4. 支持自然语言指令，例如：
   - 「前进 30 厘米」
   - 「右转 90 度再前进 20 厘米」
   - 「画一个边长 30 厘米的正方形」
   - 「左转 45 度后退 10 厘米」
5. 静音 1 秒自动结束录音，或最长录音 8 秒强制结束。
6. 识别文字发送给大模型，自动解析为动作队列并依次执行，OLED 显示 `Rec OK`。
7. 执行过程中超声波检测到障碍物（< 5cm）会立即急停并清空所有动作。

> 💡 **提示**：此模式需要 WiFi 连接，且会消耗火山引擎 ASR 和大模型调用额度。

### 模式三：蓝牙控制模式

1. 切换到 `Bluetooth` 模式。
2. 手机蓝牙连接 HC-04 模块（默认名称/密码见模块说明书）。
3. 使用支持「蓝牙串口」的 APP 发送以下格式数据包：
   - **摇杆控制**：`[joystick,序号,LV,RH]`
     - `LV`：前后方向（-100~100，正数前进）
     - `RH`：左右方向（-100~100，正数右转）
   - **按键控制**：`[key,指令码,up]`（`up` 表示按下触发）
4. 摇杆死区为 ±10，居中时自动停车。

---

## 📡 通信协议

### 固定语音指令编码（UART2 单字节）

| 指令码 | 动作 | 说明 |
|--------|------|------|
| `0x00` | 停止 | 全部舵机置中位 |
| `0x01` | 前进 | 四轮同向前进 |
| `0x02` | 后退 | 四轮同向后退 |
| `0x03` | 左转 | 左轮前进、右轮停止（差速转向） |
| `0x04` | 右转 | 右轮前进、左轮停止（差速转向） |
| `0x05` | 坐下 | 停止（预留动作） |
| `0x06` | 趴下 | 停止（预留动作） |
| `0x07` | 跳舞 | 左右交替原地转向 4 次 |
| `0xFF` | 唤醒 | 仅 AI 模式下生效，触发云端 ASR |

### 蓝牙数据包格式

所有数据包以 `[` 开头、`]` 结尾，字段间用逗号分隔：

```
[joystick,0,50,20]   → 摇杆：前进50% + 右转20%
[key,1,up]           → 按键：执行指令码1（前进）
```

### 动作队列编码（LLM 输出 / 内部执行）

大模型返回的 JSON 数组中 `action` 字段编码：

| action | 动作 | duration 单位 |
|--------|------|---------------|
| 0 | 停止 | — |
| 1 | 前进 | 毫秒（1cm ≈ 112.5ms） |
| 2 | 后退 | 毫秒（1cm ≈ 112.5ms） |
| 3 | 差速左转 | 毫秒 |
| 4 | 差速右转 | 毫秒 |
| 5 | 原地左转 | 度（目标角度，陀螺仪闭环） |
| 6 | 原地右转 | 度（目标角度，陀螺仪闭环） |
| 7 | 跳舞 | 毫秒 |

**示例**：右转 90° 再前进 20cm

```json
[{"action":6,"duration":90},{"action":1,"duration":2250}]
```

---

## 🏗️ 系统架构与工作原理

### 整体架构

```
┌─────────────────────────────────────────────────────────┐
│                    ESP32-S3 主控                         │
├──────────┬──────────┬──────────┬──────────┬─────────────┤
│  UART2   │  UART1   │   I2S    │   I2C    │   GPIO      │
│  天问模块 │  HC-04   │ INMP441  │ MPU6500  │ 超声波/按键 │
│          │  蓝牙    │  麦克风   │  OLED    │  4路舵机    │
└──────────┴──────────┴──────────┴──────────┴─────────────┘
                          │
                    ┌─────┴─────┐
                    │  WiFi     │
                    │  ASR+LLM  │
                    └───────────┘
```

### 主循环调度（非阻塞架构）

`loop()` 每 2ms 执行一次，按优先级依次处理：

1. 🛑 **超声波避障**（最高优先级，全模式生效）— 每 100ms 测距，< 5cm 立即急停。
2. 🎯 **陀螺仪姿态解算** — 每 10ms 更新航向角 Yaw，全模式运行。
3. 🔄 **按键检测** — 消抖 + 2 秒冷却，切换控制模式。
4. 💓 **WebSocket 保活** — 已连接时每 30 秒发 ping。
5. 🧠 **AI 状态机**（仅 AI 模式）— 录音 → 等结果 → 解析 → 执行。
6. 📋 **动作队列执行**（仅 AI 模式）— 非阻塞逐条执行，转向用陀螺仪闭环。
7. 🎙️ **固定语音指令**（非蓝牙模式）— 读取 UART2 字节，唤醒或执行指令。
8. 📱 **蓝牙指令**（仅蓝牙模式）— 环形缓存接收并解析数据包。

### 转向闭环控制原理

原地转向时不使用固定时长，而是实时读取 MPU6500 航向角 Yaw，与目标角度对比后分级调速：

| 剩余角度 | 速度档位 |
|----------|----------|
| > 35° | 全速（50% PWM 差量） |
| 22° ~ 35° | 中速（SPEED_MID = 20） |
| 10° ~ 22° | 精细慢速（SPEED_FINE = 8） |
| < 10° | 到达目标，立即停车 |

配合陀螺仪上电零点校准，转向角度误差可控制在 2° 以内，不受地面打滑影响。

### AI 语音识别完整链路

```
唤醒词(0xFF) → WebSocket连接ASR服务器 → 发送配置包
    → I2S麦克风分片采集音频(1024字节/片) → 滑动滤波判断音量
    → 静音1秒/满8秒 → 发送结束包 → 等待云端返回识别文字
    → HTTP调用大模型 → 解析JSON动作数组 → 入队执行
```

---

## 📁 代码结构

```
AI_car/
├── AI_car.h          # 头文件：宏定义、全局变量、函数声明
└── AI_car.cpp        # 实现文件：全部功能逻辑
```

### AI_car.h 主要分区

| 分区 | 内容 |
|------|------|
| WiFi 配置 | SSID、密码 |
| ASR 配置 | API Key、服务器地址、资源 ID |
| MPU6500 配置 | I2C 地址、寄存器、量程、姿态变量 |
| 超声波配置 | 引脚、安全距离、测距间隔 |
| 麦克风配置 | I2S 引脚、采样率、录音参数 |
| WebSocket | 客户端对象、保活参数、识别结果缓存 |
| AI 状态机 | `AiState` 枚举（空闲/录音/等结果/解析/执行） |
| 动作队列 | `ActionItem` 结构体、环形队列、最多 20 条 |
| OLED / 舵机 / 串口 | 引脚定义、硬件对象 |
| 蓝牙环形缓冲区 | 100 字节缓存、读写指针 |
| 控制模式 | `CtrlMode` 枚举（固定语音/AI/蓝牙） |

### AI_car.cpp 主要函数模块

| 模块 | 核心函数 |
|------|----------|
| MPU6500 驱动 | `mpu6500Init()` / `mpu6500CalibrateGyro()` / `updateAttitude()` |
| 超声波避障 | `getDistance()` / `checkObstacle()` |
| OLED 显示 | `updateOLED()` / `showStatus()` |
| WiFi & 麦克风 | `connectWiFi()` / `initMicrophone()` |
| ASR 语音识别 | `startStreamASR()` / `processRecording()` / `asrWebSocketEvent()` |
| LLM 解析 | `parseLLMToActions()` |
| 动作队列 | `enqueueAction()` / `processActionQueue()` / `runAction()` |
| AI 总调度 | `processAI()` |
| 模式切换 | `handleKeySwitch()` / `switchControlMode()` |
| 蓝牙通信 | `BlueSerial_*()` 系列 / `controlByJoystick()` |
| 运动控制 | `setMotorSpeed()` / `moveForward()` 等 / `processCommand()` |

---

## ⚙️ 可调参数说明

所有参数集中在 `AI_car.h` 顶部，可根据实际硬件调整：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `SAFE_DISTANCE` | 5.0 cm | 超声波安全距离，小于此值急停 |
| `ULTRA_INTERVAL` | 100 ms | 测距间隔，越小越灵敏但占用更多 CPU |
| `SAMPLE_RATE` | 16000 Hz | 麦克风采样率，ASR 标准要求 |
| `MAX_RECORD_MS` | 8000 ms | 单次最长录音时长 |
| `SILENCE_THRESHOLD` | 1200 | 静音判定音量阈值，环境嘈杂可调大 |
| `SILENCE_END_MS` | 1000 ms | 静音持续多久后结束录音 |
| `SLOW_DEG` | 35° | 转向开始减速的剩余角度 |
| `FINE_DEG` | 22° | 转向进入精细微调的剩余角度 |
| `STOP_ADVANCE` | 10° | 转向到位容忍误差 |
| `STRAIGHT_MS_PER_CM` | 135.63 ms/cm | 直行距离标定（不同地面/电池电压会有差异） |
| `YAW_DIRECTION` | 1 | 航向角方向，转向反了改为 -1 |
| `JOYSTICK_DEAD_ZONE` | 10 | 蓝牙摇杆死区 |

### 舵机中位校准

360° 连续旋转舵机的停止脉冲可能因个体差异偏移，若小车停车后仍缓慢蠕动，微调以下值：

```cpp
const int STOP_SPEED    = 95;   // 停止脉冲，微调直到完全静止
const int LEFT_FORWARD  = 85;   // 左轮前进脉冲（越小越快）
const int LEFT_BACK     = 105;  // 左轮后退脉冲（越大越快）
const int RIGHT_FORWARD = 105;  // 右轮前进脉冲
const int RIGHT_BACK    = 85;   // 右轮后退脉冲
```

---

## ❓ 常见问题 FAQ

<details>
<summary><b>Q1：上电后串口显示 MPU6500 芯片 ID 错误？</b></summary>

检查 I2C 接线（SDA→GPIO5, SCL→GPIO6），确认模块是 MPU6500（ID=0x70）而非 MPU6050（ID=0x68）。若使用 MPU6050，需修改 `mpu6500Init()` 中的 ID 判断为 `0x68`。

</details>

<details>
<summary><b>Q2：AI 模式下唤醒后一直显示 Connect Fail？</b></summary>

检查 WiFi 是否连接成功、ASR API Key 是否正确、火山引擎是否已开通流式语音识别服务并完成实名认证。

</details>

<details>
<summary><b>Q3：语音识别结果正确但小车不动？</b></summary>

检查大模型 API Key 和接入点 ID（`ep-` 开头）是否填写正确，大模型返回的 JSON 是否符合动作数组格式。可查看串口日志中 `[LLM] 原始返回内容`。

</details>

<details>
<summary><b>Q4：转向角度不准，总是偏多或偏少？</b></summary>

确认上电校准期间小车保持静止；若方向相反，修改 `YAW_DIRECTION` 为 `-1`；若角度误差大，可微调 `STOP_ADVANCE`（容忍误差）或检查陀螺仪安装是否水平。

</details>

<details>
<summary><b>Q5：超声波频繁误触发急停？</b></summary>

增大 `SAFE_DISTANCE` 或检查超声波模块供电是否稳定（需 5V 供电），避免舵机电源干扰。

</details>

<details>
<summary><b>Q6：蓝牙连接后没反应？</b></summary>

确认 HC-04 波特率为 9600（与代码 `uart1BaudRate` 一致），TX/RX 交叉连接，APP 发送的数据包格式必须包含 `[]`。

</details>

<details>
<summary><b>Q7：舵机抖动或无力？</b></summary>

舵机必须外接 5V/2A 以上独立电源，不可直接用 ESP32 的 3.3V 或 USB 供电；共地（GND 互连）不能省略。

</details>

---

## 📄 许可证

本项目采用 [MIT License](https://opensource.org/licenses/MIT) 开源协议，可自由使用、修改和分发，使用时请保留原始版权声明。

---

## 🙏 致谢

- [Espressif ESP32](https://www.espressif.com/) — 主控芯片与 Arduino 核心
- [火山引擎语音技术](https://www.volcengine.com/product/speech) — 流式 ASR 语音识别
- [火山方舟大模型平台](https://www.volcengine.com/product/ark) — 自然语言动作解析
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) — OLED 驱动库
- [WebSocketsClient](https://github.com/Links2004/arduinoWebSockets) — WebSocket 库
- [ArduinoJson](https://arduinojson.org/) — JSON 解析库
- [江协科技](https://jiangxiekeji.com) — 原生适配本项目蓝牙协议
---

<div align="center">

如果本项目对你有帮助，欢迎点个 ⭐ Star 支持！

</div>
