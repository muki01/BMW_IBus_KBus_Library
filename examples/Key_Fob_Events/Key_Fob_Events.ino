// Key_Fob_Events - reacts to the buttons of the remote key.
//
// The body module (GM) reports every key fob button to all modules. This
// example listens only to the body module and prints which button was pressed.
//
// Arduino Nano / Uno: the bus uses the hardware UART (D0 / D1) and the debug
//                     output goes to a software serial port on D7 (RX) / D8 (TX).
// ESP32:              the bus uses Serial2 and the debug output goes to USB.

#include <BMW_IBus_KBus.h>
#include <BMW_IBus_KBus_Modules.h>

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

const byte sourceFilter[] = {M_GM5};  // only messages from the body module reach packetHandler()

void setup() {
  debugSerial.begin(DEBUG_BAUD);
  debugSerial.println(F("--Key fob events--"));

  ibus.setPins(SEN_STA_PIN, ENABLE_PIN, LED_PIN);
  ibus.setIbusSerial(busSerial);
  ibus.setSourceFilter(sourceFilter, sizeof(sourceFilter));
  ibus.setIbusPacketHandler(packetHandler);
  attachInterrupt(digitalPinToInterrupt(SEN_STA_PIN), startTimer, CHANGE);
}

void loop() {
  ibus.run();
}

void packetHandler(byte *packet) {
  byte source = packet[0];
  byte destination = packet[2];
  byte command = packet[3];
  byte data = packet[4];

  if (source == M_GM5 && destination == M_ALL && command == 0x72) {  // 00 04 BF 72 xx
    if (data == 0x12) {
      debugSerial.println(F("Lock button pressed"));
    } else if (data == 0x22) {
      debugSerial.println(F("Unlock button pressed"));
    } else if (data == 0x42) {
      debugSerial.println(F("Trunk button pressed"));
    }
  }
}
