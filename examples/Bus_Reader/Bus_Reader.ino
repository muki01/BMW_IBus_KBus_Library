// Bus_Reader - prints every message on the I/K-Bus to the debug port.
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

uint8_t source, length, destination, databytes[36];

void setup() {
  debugSerial.begin(DEBUG_BAUD);
  debugSerial.println(F("--IBUS reader--"));

  ibus.setPins(SEN_STA_PIN, ENABLE_PIN, LED_PIN);
  ibus.setIbusSerial(busSerial);
  ibus.setIbusDebug(debugSerial);  // prints every message as hex
  ibus.setIbusPacketHandler(packetHandler);
  attachInterrupt(digitalPinToInterrupt(SEN_STA_PIN), startTimer, CHANGE);
}

void loop() {
  ibus.run();
}

void packetHandler(byte *packet) {
  source = packet[0];
  length = packet[1];
  destination = packet[2];
  for (int i = 0, s = 3; i <= length - 3; i++, s++) {
    databytes[i] = packet[s];
  }
}
