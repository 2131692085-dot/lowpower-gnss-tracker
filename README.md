# 畜牧牛羊定位器（GPS 定位终端 + W5500 网关）

基于 STM32F103 的物联网定位终端及以太网网关项目，面向畜牧养殖场景，实现牛羊位置的采集、封装、上报与汇聚。

## 项目架构

```
[AT6558R GPS] --NMEA--> [STM32F103 定位终端] --cJSON/NB-IoT(QS100)--> 云端
                              |
                              +--LoRa--> [STM32F103 网关] --W5500(SPI)--> 以太网/上位机
```

- **定位终端**：AT6558R GPS 模块采集定位信息，解析 NMEA 语句后以 cJSON 封装，经 NB-IoT（QS100 模组）上报云平台；支持 LoRa 远距离通信链路；采用 RTC 定时唤醒 + STOP 模式低功耗调度，适配野外电池供电场景。
- **网关**：STM32F103ZET6 双 SPI 架构（SPI1 + SPI2），基于 W5500 硬件协议栈完成以太网接入，实现定位终端数据的汇聚与转发。

## 工程内容

本仓库为项目最终完成版（10 个开发阶段的最终形态）：

- `Core/` — CubeMX 生成的外设初始化（SPI1/SPI2、USART1、RTC、NVIC）
- `MDK-ARM/` — Keil MDK5 工程文件（STM32F103ZET6，启动文件 startup_stm32f103xe.s）
- `MDK-ARM/source/` — 应用层代码（App_Main）

## 开发过程要点

项目按 10 个阶段递进开发：GPS 点亮 → 串口通信 → DS3553 步进电机联动 → AT6558R 接入 → NMEA 解析 → cJSON 数据封装 → QS100 NB-IoT 联网 → RTC 低功耗 → LoRa 链路 → W5500 以太网网关。

## 构建与烧录

1. 使用 Keil MDK5 打开 `MDK-ARM/atguigu-project-location-gateway.uvprojx`
2. 选择 STM32F103ZE 目标板编译（需安装 STM32F1xx DFP）
3. ST-Link SWD 烧录

## 涉及技术

STM32 HAL / FreeRTOS 思想 / GPS NMEA-0183 解析 / cJSON / NB-IoT AT 指令 / LoRa / W5500 SPI 以太网 / RTC STOP 低功耗
