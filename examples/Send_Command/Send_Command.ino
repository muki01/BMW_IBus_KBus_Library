// Send_Command - sends a message to the car every 10 seconds.
//
// The example message flashes the red alarm LED under the interior mirror for
// 3 seconds (BMW E46). The library waits until the bus is idle before sending.
//
// Arduino Nano / Uno: the bus uses the hardware UART (D0 / D1) and the debug
//                     output goes to a software serial port on D7 (RX) / D8 (TX).
// ESP32:              the bus uses Serial2 and the debug output goes to USB.

#include <BMW_IBus_KBus.h>

#if defined(ESP32)
#define busSerial Serial2    // RX2 = GPIO16, TX2 = GPIO17
#define debugSerial Serial   // USB
const unsigned long DEBUG_BAUD = 115200;
const byte SEN_STA_PIN = 4;  // transceiver SEN/STA
const byte ENABLE_PIN = 5;   // transceiver EN
const byte LED_PIN = 2;      // bus activity LED
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
