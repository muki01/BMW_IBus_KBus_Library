<div align="center">

<img src="images/bmw-ibus-kbus-arduino-library-banner.svg" alt="BMW IBus KBus — Arduino library for the BMW I-Bus and K-Bus, showing a decoded frame 50 04 68 32 11 1F" width="100%">

# BMW IBus KBus — Arduino Library

**Read, decode and send BMW I-Bus and K-Bus messages from your own sketch.**<br>
A compact Arduino library that handles framing, checksums and bus arbitration, so you can sniff bus traffic or control lights, windows, locks and radio on classic BMWs — E46, E39, E38, E53 and more.

[![Stars](https://img.shields.io/github/stars/muki01/BMW_IBus_KBus_Library?style=flat-square&logo=github&color=ff8a2a)](https://github.com/muki01/BMW_IBus_KBus_Library/stargazers)
[![Forks](https://img.shields.io/github/forks/muki01/BMW_IBus_KBus_Library?style=flat-square&logo=github&color=38bdf8)](https://github.com/muki01/BMW_IBus_KBus_Library/forks)
[![Issues](https://img.shields.io/github/issues/muki01/BMW_IBus_KBus_Library?style=flat-square)](https://github.com/muki01/BMW_IBus_KBus_Library/issues)
[![License: MIT](https://img.shields.io/github/license/muki01/BMW_IBus_KBus_Library?style=flat-square)](LICENSE)
[![Build](https://img.shields.io/github/actions/workflow/status/muki01/BMW_IBus_KBus_Library/arduino-ci.yml?style=flat-square&label=build)](https://github.com/muki01/BMW_IBus_KBus_Library/actions/workflows/arduino-ci.yml)
![Arduino](https://img.shields.io/badge/Arduino-00979D?style=flat-square&logo=arduino&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-experimental-E7352C?style=flat-square&logo=espressif&logoColor=white)

[Installation](#-installation) ·
[Quick Start](#-quick-start) ·
[API](#-api-reference) ·
[Wiring](#-wiring) ·
[Examples](#-examples) ·
[Where to Connect](#-where-to-connect-bmw-e46) ·
[Main Project](https://github.com/muki01/BMW_IBus_KBus)

</div>

---

## 🌟 Overview

Classic BMWs link their body and infotainment modules over a **single 12 V wire**: the **I-Bus** and **K-Bus**. Every key-fob click, steering-wheel button and light switch travels across it as a short serial message at **9600 baud, 8E1**.

**BMW IBus KBus** is the library that puts your microcontroller on that bus. You register a callback, call `run()` in your loop, and the library does the rest.

```mermaid
flowchart LR
    CAR["BMW I-Bus / K-Bus<br/>12 V · single wire"] <--> TRX["Bus transceiver<br/>TH3122.4 · ELMOS 10026B"]
    TRX <-->|"UART · 9600 8E1"| LIB["BMW IBus KBus<br/>library"]
    LIB -->|"packet handler"| APP["Your sketch"]
    APP -->|"write()"| LIB
```

Looking for ready-to-flash firmware? The companion project **[BMW_IBus_KBus](https://github.com/muki01/BMW_IBus_KBus)** has complete sketches for the Arduino Nano and the ESP32 — welcome lights, follow-me-home and a web interface — together with an E46 message table.

## ✨ Features

- 📥 **Reliable receiving** — a state machine finds frame boundaries in the byte stream and verifies every checksum.
- 📤 **Safe transmitting** — messages are queued and sent only after the bus has been idle, so they never collide with the car's own traffic.
- 🧮 **Automatic checksums** — define a message without its checksum and the library appends it, or send a complete frame unchanged.
- 🎯 **Source filter** — receive everything, or only the modules you care about.
- 💤 **Sleep support** — powers the transceiver down after a period of bus silence, like the car's own modules.
- 🔎 **Built-in debug output** — every received and transmitted frame as hex on any `Stream`.
- 📍 **Configurable pins** and an optional header with more than 80 named module addresses.

## 📡 Supported Vehicles

| Chassis | Series | Years | I-Bus | K-Bus |
| :-- | :-- | :-- | :-: | :-: |
| **E31** | 8 Series | 1989–1999 | ✅ | |
| **E38** | 7 Series | 1994–2001 | ✅ | ✅ |
| **E39** | 5 Series | 1995–2004 | ✅ | ✅ |
| **E46** | 3 Series | 1997–2006 | | ✅ |
| **E52** | Z8 | 2000–2003 | | ✅ |
| **E53** | X5 | 1999–2006 | ✅ | ✅ |
| **E83** | X3 | 2003–2010 | | ✅ |
| **E85** | Z4 | 2002–2008 | | ✅ |
| **E87** | 1 Series | 2004–2013 | | ✅ |

## 📦 Installation

**Arduino IDE** — download this repository as a ZIP (**Code → Download ZIP**), then choose **Sketch → Include Library → Add .ZIP Library…**

**Manual** — clone it into your Arduino `libraries` folder:

```bash
git clone https://github.com/muki01/BMW_IBus_KBus_Library.git
```

**PlatformIO** — add it to `platformio.ini`:

```ini
lib_deps = https://github.com/muki01/BMW_IBus_KBus_Library.git
```

## 🚀 Quick Start

```cpp
#include <BMW_IBus_KBus.h>
#include <SoftwareSerial.h>

SoftwareSerial debugSerial(7, 8);          // RX, TX — to a USB-serial adapter
BMW_IBus_KBus ibus;

void startTimer() {                        // called on every SEN/STA level change
  ibus.startTimer();
}

void setup() {
  debugSerial.begin(9600);
  ibus.setIbusSerial(Serial);              // bus on the hardware UART (9600 8E1)
  ibus.setIbusDebug(debugSerial);          // print every frame as hex
  ibus.setIbusPacketHandler(packetHandler);
  attachInterrupt(digitalPinToInterrupt(3), startTimer, CHANGE);
}

void loop() {
  ibus.run();
}

void packetHandler(byte *packet) {
  byte source      = packet[0];            // sender module
  byte length      = packet[1];            // bytes that follow (destination … checksum)
  byte destination = packet[2];            // target module
  byte command     = packet[3];            // message type, followed by data bytes

  if (source == 0x50 && destination == 0x68 && command == 0x32) {
    // steering wheel → radio: volume button
  }
}
```

Example debug output:

```text
Good Message -> 50 04 68 32 11 1F
Good Message -> 00 04 BF 72 22 EB
Good Message -> 80 04 BF 11 01 2B
```

> [!TIP]
> On the Arduino Nano and Uno the bus uses the hardware UART (D0 / D1), which is shared with USB. Disconnect the transceiver from D0 / D1 while uploading.

### Sending a message

Store the message in `PROGMEM` **without** its checksum. The library calculates and appends the checksum, then transmits as soon as the bus is idle.

```cpp
// diagnostic interface → body module: switch the interior light on
const byte Interior_On[] PROGMEM = {0x3F, 0x05, 0x00, 0x0C, 0x01, 0x01};

ibus.write(Interior_On, sizeof(Interior_On));
```

If a message already ends with its checksum, pass `false` as the third argument and it is sent unchanged:

```cpp
const byte Interior_On_Full[] PROGMEM = {0x3F, 0x05, 0x00, 0x0C, 0x01, 0x01, 0x36};

ibus.write(Interior_On_Full, sizeof(Interior_On_Full), false);
```

### Filtering by module

By default every valid message reaches your packet handler. To receive only certain modules, pass their addresses:

```cpp
#include <BMW_IBus_KBus_Modules.h>

const byte sourceFilter[] = {M_GM5, M_MFL, M_RAD};   // body module, steering wheel, radio

ibus.setSourceFilter(sourceFilter, sizeof(sourceFilter));
```

## 📘 API Reference

| Method | Description |
| :-- | :-- |
| `setIbusSerial(serial)` | Attach the bus to a hardware UART and configure it for 9600 8E1. |
| `setIbusPacketHandler(fn)` | Register the function called for every valid message: `void fn(byte *packet)`. |
| `setIbusDebug(stream)` | Mirror all received and transmitted messages to any `Stream` as hex. |
| `setPins(senSta, enable, led)` | Select the SEN/STA, EN and activity-LED pins. Defaults: `3`, `4`, `13`. |
| `setSourceFilter(sources, count)` | Pass only messages from the listed modules to the handler. `count` `0` disables the filter. |
| `write(message, size)` | Queue a `PROGMEM` message; the checksum is calculated and appended. |
| `write(message, size, false)` | Queue a `PROGMEM` message that already includes its checksum. |
| `sleepEnable(seconds)` | Pull EN low after this many seconds without a valid message. |
| `startTimer()` | Call from the SEN/STA pin-change interrupt to track bus-idle time. |
| `run()` | Call continuously from `loop()` — receives, parses and transmits. |
| `calculateChecksum(data, length)` | Return the XOR checksum of a buffer in RAM. |

The packet passed to the handler is the complete frame:

| Index | Field | Description |
| :-- | :-- | :-- |
| `packet[0]` | Source | Sender module address |
| `packet[1]` | Length | Number of bytes that follow |
| `packet[2]` | Destination | Target module address |
| `packet[3]` | Command | Message type |
| `packet[4…]` | Data | Payload, followed by the checksum |

## 🔌 Wiring

The bus idles at battery voltage — never connect it directly to a microcontroller pin. The library is designed around the **TH3122.4 / ELMOS 10026B** bus transceiver:

<img src="images/TH3122.4%20or%20ELMOS%2010026B.png" alt="BMW I-Bus K-Bus transceiver schematic with TH3122.4 or ELMOS 10026B for Arduino" width="75%">

| Transceiver pin | Arduino Nano / Uno | Purpose |
| :-- | :-- | :-- |
| TXD | `D0` (RX) | Bus → microcontroller |
| RXD | `D1` (TX) | Microcontroller → bus |
| SEN/STA | `D3` | Bus-idle detection before transmitting |
| EN | `D4` | Transceiver enable / sleep |
| — | `D13` | Bus activity LED |

Use `setPins()` when your board needs different pins. SEN/STA must be connected to an interrupt-capable pin.

<details>
<summary><b>Alternative interface circuits</b></summary>

<br>

**Optocouplers (PC817)** — built from common parts; well suited to sniffing.

<img src="images/Optocoupler%20Schematic.png" alt="BMW K-Bus optocoupler interface schematic with PC817 and BC547" width="75%">

**MCP2025 LIN transceiver** — compact, with a built-in voltage regulator.

<img src="images/MCP2025.png" alt="BMW I-Bus K-Bus interface schematic with MCP2025 LIN transceiver" width="75%">

These circuits have no SEN/STA output. Receiving works as shown; for transmitting, the library expects a bus-idle signal on the SEN/STA pin.

</details>

### Sleep mode

With the TH3122 / ELMOS transceiver powering the microcontroller from its 5 V regulator, `sleepEnable(60)` pulls EN low after 60 seconds of bus silence. The transceiver switches the regulator off, and bus activity wakes everything up again.

## 🧩 Platform Support

| Platform | Status |
| :-- | :-- |
| Arduino Nano / Uno / Mega (ATmega328P, ATmega2560) | ✅ Reference platform — developed on an Arduino Nano in a BMW E46 |
| ESP32 | 🧪 Experimental — compiles on Arduino-ESP32 2.x and 3.x, not yet verified in a vehicle |
| Raspberry Pi Pico, STM32, UNO R4 | 🧪 Experimental — compiles, not yet verified in a vehicle |

On ESP32 the examples use `Serial2` for the bus and USB for debug output. Mark your interrupt function with `IBUS_ISR_ATTR` so that it is placed in RAM:

```cpp
void IBUS_ISR_ATTR startTimer() {
  ibus.startTimer();
}
```

If you run the library on one of the experimental platforms, please [report how it went](https://github.com/muki01/BMW_IBus_KBus_Library/issues).

## 🧪 Examples

| Example | What it shows |
| :-- | :-- |
| [`Bus_Reader`](examples/Bus_Reader) | Print every message on the bus — start here. |
| [`Send_Command`](examples/Send_Command) | Send a message to the car, with and without a pre-calculated checksum. |
| [`Key_Fob_Events`](examples/Key_Fob_Events) | Filter by module and react to the lock, unlock and trunk buttons of the remote key. |

Complete vehicle firmware — welcome lights, goodbye lights, follow-me-home and an ESP32 web interface — and a table of more than 100 E46 messages live in the [companion project](https://github.com/muki01/BMW_IBus_KBus).

## 📍 Where to Connect (BMW E46)

The I/K-Bus is **not** available on the OBD-II port. The easiest access point is the **CD-changer connector** in the trunk, which is pre-wired on most cars and provides power, ground and the bus in one plug.

<table>
  <tr>
    <td width="33%"><img src="images/bmw-e46-trunk-cd-changer-location.jpg" alt="BMW E46 trunk, left side trim panel hiding the CD changer wiring"></td>
    <td width="33%"><img src="images/bmw-e46-cd-changer-bracket-wiring.jpg" alt="BMW E46 CD changer bracket with the pre-wired K-Bus connector behind the trunk trim"></td>
    <td width="33%"><img src="images/bmw-e46-cd-changer-connector-x18180-kbus.jpg" alt="BMW E46 CD changer connector X18180 with K-Bus, 12 V and ground wires"></td>
  </tr>
  <tr>
    <td align="center"><b>1.</b> Open the trunk — driver's side</td>
    <td align="center"><b>2.</b> Remove the trim to reach the bracket</td>
    <td align="center"><b>3.</b> The 3-pin connector <b>X18180</b></td>
  </tr>
</table>

| Wire colour | Signal |
| :-- | :-- |
| ⚪🔴🟡 White / red with yellow dots | **K-Bus** |
| 🔴🟢 Red / green | **+12 V** |
| 🟤 Brown | **Ground** |

<details>
<summary><b>Alternative: the K-Bus junction block above the fuse box</b></summary>

<br>

<table>
  <tr>
    <td width="50%"><img src="images/bmw-e46-fuse-box-kbus-junction-location.jpg" alt="BMW E46 fuse box with the K-Bus junction block above it"></td>
    <td width="50%"><img src="images/bmw-e46-kbus-junction-connector-removed.jpg" alt="BMW E46 connector block removed from above the fuse box"></td>
  </tr>
  <tr>
    <td align="center"><b>1.</b> Locate the connector block above the fuse box</td>
    <td align="center"><b>2.</b> Unclip it and pull it out</td>
  </tr>
  <tr>
    <td><img src="images/bmw-e46-kbus-junction-block-terminals.jpg" alt="BMW E46 K-Bus junction block with terminals marked by arrows"></td>
    <td><img src="images/bmw-e46-kbus-wires-white-red-yellow.jpg" alt="BMW E46 K-Bus wires, white and red with yellow dots, joined by a comb connector"></td>
  </tr>
  <tr>
    <td align="center"><b>3.</b> Find the K-Bus junction block</td>
    <td align="center"><b>4.</b> All white / red / yellow wires are K-Bus</td>
  </tr>
</table>

</details>

## 📨 Message Format

```text
 50    04    68    32    11    1F
 │     │     │     │     │     └─ Checksum     XOR of all previous bytes
 │     │     │     │     └─────── Data         one or more bytes
 │     │     │     └───────────── Command      message type
 │     │     └─────────────────── Destination  0x68 = radio
 │     └───────────────────────── Length       number of bytes that follow
 └─────────────────────────────── Source       0x50 = multifunction steering wheel
```

Include `BMW_IBus_KBus_Modules.h` to use names such as `M_MFL`, `M_RAD`, `M_IKE` and `M_LCM` instead of raw addresses.

## 🔄 Upgrading from IbusSerial

Sketches written for the earlier `IbusSerial` version still compile — `#include <IbusSerial.h>` and the `IbusSerial` class name are kept as aliases. Two behaviours changed:

- **`write()` now appends the checksum.** Remove the checksum byte from your message arrays, or pass `false` as the third argument.
- **All modules are forwarded by default.** The previous version passed on only a fixed list of modules. Use `setSourceFilter()` to restore a filter.

## 🔗 Related Projects

| Firmware & Readers | Libraries | Manufacturer Protocols | UI |
| :-- | :-- | :-- | :-- |
| [OBD2 K-line Reader](https://github.com/muki01/OBD2_K-line_Reader) | [OBD2 K-Line Library](https://github.com/muki01/OBD2_KLine_Library) | [BMW I/K Bus](https://github.com/muki01/BMW_IBus_KBus) | [OBD2 Diagnostic UI](https://github.com/muki01/OBD2-Diagnostic-UI) |
| [OBD2 CAN Bus Reader](https://github.com/muki01/OBD2_CAN_Bus_Reader) | [OBD2 CAN Bus Library](https://github.com/muki01/OBD2_CAN_Bus_Library) | [VAG KW1281](https://github.com/muki01/VAG_KW1281) | |

## ☕ Support the Project

[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/muki01)
[![PayPal](https://img.shields.io/badge/PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/donate/?hosted_button_id=SAAH5GHAH6T72)
[![GitHub Sponsors](https://img.shields.io/badge/GitHub%20Sponsors-181717?style=for-the-badge&logo=github)](https://github.com/sponsors/muki01)

For custom automotive firmware, hardware or protocol work, contact 📧 **[muksin.muksin04@gmail.com](mailto:muksin.muksin04@gmail.com)**.

## ⚠️ Disclaimer

> [!WARNING]
> This is a hobby and development project. Transmitting on a live vehicle bus can affect lighting, locking and other body functions. Test with the vehicle stationary and proceed at your own risk. The author accepts no responsibility for damage or malfunction.

BMW is a registered trademark of BMW AG. This project is independent and is not affiliated with, endorsed by or sponsored by BMW AG.

## 📄 License

Released under the [MIT License](LICENSE).

---

<div align="center">

Created by [**Muki**](https://github.com/muki01) · If this library helped you, please give it a ⭐

</div>
