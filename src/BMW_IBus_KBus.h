#ifndef BMW_IBus_KBus_h
#define BMW_IBus_KBus_h

#include "Arduino.h"
#include "IbusRingBuffer.h"

#define IBUS_DEBUG

// Functions called from an interrupt must live in RAM on ESP boards
#if defined(ESP32) || defined(ESP8266)
#define IBUS_ISR_ATTR IRAM_ATTR
#else
#define IBUS_ISR_ATTR
#endif

class BMW_IBus_KBus {
 public:
  BMW_IBus_KBus();
  ~BMW_IBus_KBus();
  void setIbusSerial(HardwareSerial &newIbusSerial);
#if defined(IBUS_DEBUG)
  void setIbusDebug(Stream &newIbusDebug);
#endif
  void setPins(byte newSenStaPin, byte newEnablePin, byte newLedPin);
  void setSourceFilter(const byte sources[], byte count);
  void run();
  void write(const byte message[], byte size, bool addChecksum = true);
  void printDebugMessage(const char *debugPrefix);
  typedef void IbusPacketHandler_t(byte *packet);
  void setIbusPacketHandler(IbusPacketHandler_t newHandler);
  void contentionTimer();
  void startTimer();
  void sleepEnable(unsigned long sleepTime);
  byte calculateChecksum(const byte *data, byte length);
  void sleep();

 private:
  enum ibusFsmStates {
    FIND_SOURCE,    // Read source byte
    FIND_LENGTH,    // Read length byte
    FIND_MESSAGE,   // Read in main body of message
    GOOD_CHECKSUM,  // Process message if checksum good
    BAD_CHECKSUM,   // Debug print bad message if checksum bad
  } ibusState;

  void initPins();
  void readIbus();
  boolean sourceAllowed(byte sourceId);
  void compareIbusPacket();
  void sendIbusPacket();
  void sendIbusMessageIfAvailable();
  void updateClearToSend();
  HardwareSerial *Ibus_Serial;
#if defined(IBUS_DEBUG)
  Stream *IbusDebug;
#endif
  byte source;
  byte length;
  byte destination;
  byte ibusByte[40];
  boolean busInSync;
  static const byte packetGap = 10;
  byte *pData;
  IbusPacketHandler_t *pIbusPacketHandler;
  const byte *filterSources;
  byte filterCount;
};

typedef BMW_IBus_KBus IbusSerial;  // previous name of the class

#endif
