// Send_Command - sends a message to the car every 10 seconds.
//
// The example message flashes the red alarm LED under the interior mirror for
// 3 seconds (BMW E46). The library waits until the bus is idle before sending.
//
// Arduino Nano / Uno: the bus uses the hardware UART (D0 / D1) and the debug
//                     output goes to a software serial port on D7 (RX) / D8 (TX).
// ESP32:              the bus uses Serial1 on the pins below and the debug output
//                     goes to USB.

#include <BMW_IBus_KBus.h>

#if defined(ESP32)
#define busSerial Serial1    // opened on BUS_RX_PIN / BUS_TX_PIN in setup()
#define debugSerial Serial   // USB
const unsigned long DEBUG_BAUD = 115200;
#if defined(CONFIG_IDF_TARGET_ESP32)  // classic ESP32: GPIO 6-11 belong to the flash chip
const byte BUS_RX_PIN = 16;  // transceiver TXD -> ESP32
const byte BUS_TX_PIN = 17;  // ESP32 -> transceiver RXD
const byte SEN_STA_PIN = 4;  // transceiver SEN/STA
const byte ENABLE_PIN = 5;   // transceiver EN
const byte LED_PIN = 2;      // bus activity LED
#else                                 // ESP32-C6, ESP32-C3, ESP32-S3 ...
const byte BUS_RX_PIN = 5;   // transceiver TXD -> ESP32
const byte BUS_TX_PIN = 4;   // ESP32 -> transceiver RXD
const byte SEN_STA_PIN = 7;  // transceiver SEN/STA
const byte ENABLE_PIN = 3;   // transceiver EN
#if defined(CONFIG_IDF_TARGET_ESP32C3)
const byte LED_PIN = 8;      // bus activity LED (GPIO 12-17 belong to the flash chip on the ESP32-C3)
#else
const byte LED_PIN = 15;     // bus activity LED
#endif
#endif
#else
#include <SoftwareSerial.h>
SoftwareSerial debugSerial(7, 8);  // RX, TX
#define busSerial Serial
const unsigned long DEBUG_BAUD = 9600;
const byte SEN_STA_PIN = 3;  // transceiver SEN/STA
const byte ENABLE_PIN = 4;   // transceiver EN
const byte LED_PIN = 13;     // bus activity LED
#endif

BMW_IBus_KBus ibus;

// Called on every level change of the SEN/STA pin
void IBUS_ISR_ATTR startTimer() {
  ibus.startTimer();
}

// Message without checksum: the library calculates and appends it
const byte AlarmLedFlash[] PROGMEM = {0x3F, 0x05, 0x00, 0x0C, 0x4E, 0x01};

// The same message with its checksum already included
const byte AlarmLedFlashFull[] PROGMEM = {0x3F, 0x05, 0x00, 0x0C, 0x4E, 0x01, 0x79};

unsigned long lastSendTime = 0;

void setup() {
  debugSerial.begin(DEBUG_BAUD);
  debugSerial.println(F("--IBUS sender--"));

  ibus.setPins(SEN_STA_PIN, ENABLE_PIN, LED_PIN);
#if defined(ESP32)
  busSerial.begin(9600, SERIAL_8E1, BUS_RX_PIN, BUS_TX_PIN);  // choose the pins before the library opens the port
#endif
  ibus.setIbusSerial(busSerial);
  ibus.setIbusDebug(debugSerial);
  ibus.setIbusPacketHandler(packetHandler);
  attachInterrupt(digitalPinToInterrupt(SEN_STA_PIN), startTimer, CHANGE);
}

void loop() {
  ibus.run();

  if (millis() - lastSendTime >= 10000) {
    lastSendTime = millis();
    ibus.write(AlarmLedFlash, sizeof(AlarmLedFlash));
    // ibus.write(AlarmLedFlashFull, sizeof(AlarmLedFlashFull), false);  // checksum already in the message
  }
}

void packetHandler(byte *packet) {
}
