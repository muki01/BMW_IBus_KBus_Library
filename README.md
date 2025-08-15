# 🚙BMW I/K Bus Library

![GitHub forks](https://img.shields.io/github/forks/muki01/I-K_Bus_Library?style=flat)
![GitHub Repo stars](https://img.shields.io/github/stars/muki01/I-K_Bus_Library?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/I-K_Bus_Library?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/I-K_Bus_Library?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/I-K_Bus_Library)

**I/K Bus Library** is a lightweight yet powerful Arduino-compatible library that enables direct communication with BMW Cars using the **I/K Bus**.

This library is designed for microcontrollers such as **Arduino**, **ESP32**, and similar platforms. It allows your device to communicate directly with a vehicle that uses the **I/K Bus**.
With this code you can read sensor values, control lights, windows and much more.
I shared schematics with optocouplers and chips. I used Arduino nano as microcontroller, but you can use another microcontrollers like STM32, ESP32 and much more.

I will share more information about I/K Bus protocol and communication later. Stay tuned 😉.

You can also see my other car projects:
1. [Тhis](https://github.com/muki01/I-K_Bus) project is for BMW with I/K bus system. 
2. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Reader) project is for Cars with CAN Bus.
3. [Тhis](https://github.com/muki01/OBD2_K-line_Reader) project is for Cars with ISO9141 and ISO14230 protocols.
4. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Library) is my OBD2 CAN Bus Communication Library for Arduino IDE.
5. [Тhis](https://github.com/muki01/OBD2_KLine_Library) is my OBD2 K-Line Communication Library for Arduino IDE.
6. [Тhis](https://github.com/muki01/I-K_Bus_Library) is my I/K Bus Communication Library for Arduino IDE.


> [!WARNING] 
> I am not responsible for any issues or damages that may occur during your testing. Please proceed with caution and at your own risk.

## 📡Supported BMW Models
This protocol applies to the bus system in the models listed below.

Model|Series|Period|I-Bus|K-Bus
:--|:--|:--|:--|:--
E31|8 Series|1989 - 1999|✅|
E38|7 Series|1999 - 2001|✅|✅
E39|5 Series|1995 - 2004|✅|✅
E46|3 Series|1997 - 2006||✅
E52|Z8|2000 - 2003||✅
E53|X5|1999 - 2006|✅|✅|
E83|X3|2003 - 2010||✅
E85|Z4|2002 - 2008||✅
E87|1 series|2004 - 2013||✅

## 🔧 Installation

1. Clone this repository or download the `.zip` file
2. Open the `.ino` file with Arduino IDE
3. Select your board (e.g., Arduino Nano, ESP32)
4. Set the correct COM port and upload the code

## 🛠️Schematics for communication

These schematics are essential because I/K Bus communication operates at different voltage and signal levels than microcontroller pins.  
The circuits ensure proper level shifting and protection for safe, stable operation.

You can choose one of the following approaches depending on your project:

### 🔹Schematic with Optocouplers
<img src="https://github.com/muki01/I-K_Bus/blob/main/Schematics/Optocoupler%20Schematic.png" width=60%>

### 🔹Schematic with TH3122.4 or ELMOS 10026B
<img src="https://github.com/muki01/I-K_Bus/blob/main/Schematics/TH3122.4%20or%20ELMOS%2010026B.png" width=60%>

### 🔹Schematic with MCP2025
<img src="https://github.com/muki01/I-K_Bus/blob/main/Schematics/MCP2025.png" width=60%>

---

## ☕ Support My Work

If you enjoy my projects and want to support me, you can do so through the links below:

[![Buy Me A Coffee](https://img.shields.io/badge/-Buy%20Me%20a%20Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/muki01)
[![PayPal](https://img.shields.io/badge/-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/donate/?hosted_button_id=SAAH5GHAH6T72)
[![GitHub Sponsors](https://img.shields.io/badge/-Sponsor%20Me%20on%20GitHub-181717?style=for-the-badge&logo=github)](https://github.com/sponsors/muki01)

📧 **Contact:** `muksin.muksin04@gmail.com`

---
