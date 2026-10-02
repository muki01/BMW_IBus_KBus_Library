#include "BMW_IBus_KBus.h"

// Boards with Timer2 (ATmega328P, ATmega2560, ...) measure the bus idle time with a
// timer interrupt. All other boards measure it with micros().
#if defined(__AVR__) && defined(TCCR2A) && defined(TIMSK2)
#define IBUS_USE_TIMER2
#endif

static byte senStaPin = 3;
static byte enablePin = 4;
static byte ledPin = 13;
static volatile boolean clearToSend = false;
static unsigned long ibusTimeToSleep;
static boolean sleepEnabled;
static unsigned long ibusSleepTime = 0;
static unsigned long packetTimer = 0;
static IbusRingBuffer ibusReceiveBuffer(128);
static IbusRingBuffer ibusSendBuffer(64);

#if !defined(IBUS_USE_TIMER2)
static volatile boolean busIdle = true;
static volatile unsigned long busIdleSince = 0;
static const unsigned long contentionTime = 1500;  // microseconds of bus silence before sending
#endif

BMW_IBus_KBus::BMW_IBus_KBus() {  // constructor
  busInSync = false;
  ibusSleepTime = millis();
  packetTimer = millis();
  source = 0;
  length = 0;
  destination = 0;
  ibusState = FIND_SOURCE;
  sleepEnabled = false;
  Ibus_Serial = NULL;
#if defined(IBUS_DEBUG)
  IbusDebug = NULL;
#endif
  pIbusPacketHandler = NULL;
  filterSources = NULL;
  filterCount = 0;
#if defined(IBUS_USE_TIMER2)
  initPins();
  contentionTimer();
#endif
}

BMW_IBus_KBus::~BMW_IBus_KBus() {  // deconstructor
}

void BMW_IBus_KBus::initPins() {
  pinMode(ledPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  pinMode(senStaPin, INPUT);
}

void BMW_IBus_KBus::setPins(byte newSenStaPin, byte newEnablePin, byte newLedPin) {
  senStaPin = newSenStaPin;
  enablePin = newEnablePin;
  ledPin = newLedPin;
  initPins();
}

void BMW_IBus_KBus::setIbusSerial(HardwareSerial &newIbusSerial) {
  Ibus_Serial = &newIbusSerial;
  Ibus_Serial->begin(9600, SERIAL_8E1);  // ibus always 9600 8E1
#if !defined(IBUS_USE_TIMER2)
  initPins();  // boards without Timer2 are set up here instead of in the constructor
#endif
}

#if defined(IBUS_DEBUG)
void BMW_IBus_KBus::setIbusDebug(Stream &newIbusDebug) {
  IbusDebug = &newIbusDebug;
}
#endif

void BMW_IBus_KBus::setIbusPacketHandler(IbusPacketHandler_t newHandler) {
  pIbusPacketHandler = newHandler;
}

void BMW_IBus_KBus::setSourceFilter(const byte sources[], byte count) {
  filterSources = sources;  // only messages from these modules reach the packet handler
  filterCount = count;      // 0 = no filter, every message is passed on
}

boolean BMW_IBus_KBus::sourceAllowed(byte sourceId) {
  if (filterCount == 0) {
    return true;
  }
  for (byte i = 0; i < filterCount; i++) {
    if (filterSources[i] == sourceId) {
      return true;
    }
  }
  return false;
}

void BMW_IBus_KBus::readIbus() {
  if (Ibus_Serial->available()) {
    ibusReceiveBuffer.write(Ibus_Serial->read());
  }
  switch (ibusState) {
    case FIND_SOURCE:
      if (ibusReceiveBuffer.available() >= 1) {
        source = ibusReceiveBuffer.peek(0);
        ibusState = FIND_LENGTH;
      }
      break;

    case FIND_LENGTH:
      if (ibusReceiveBuffer.available() >= 2) {
        length = ibusReceiveBuffer.peek(1);
        if (length >= 0x03 && length <= 0x24) {  // Check if length byte between decimal 3 & 36
          ibusState = FIND_MESSAGE;
        } else {
          ibusReceiveBuffer.remove(1);  // remove source byte and start over
        }
      }
      break;

    case FIND_MESSAGE:
      if (ibusReceiveBuffer.available() >= length + 2) {  // Check if enough bytes in buffer to complete message (based on length byte)
        byte checksumByte = 0;
        for (int i = 0; i <= length; i++) {
          checksumByte ^= ibusReceiveBuffer.peek(i);
        }
        if (ibusReceiveBuffer.peek(length + 1) == checksumByte) {
          ibusSleepTime = millis();  // Restart sleep timer
          ibusState = GOOD_CHECKSUM;
        } else {
          ibusState = BAD_CHECKSUM;
        }
      }
      break;

    case GOOD_CHECKSUM:
      // only process messages we're interested in
      if (!sourceAllowed(source)) {
#ifdef IBUS_DEBUG
        if (IbusDebug != NULL) {
          IbusDebug->print(F("DISCARDED: "));
          for (int i = 0; i <= length + 1; i++) {
            if (ibusReceiveBuffer.peek(i) < 0x10) {
              IbusDebug->print(F("0"));
            }
            IbusDebug->print(ibusReceiveBuffer.peek(i), HEX);
            IbusDebug->print(F(" "));
          }
          IbusDebug->println();
        }
#endif
        ibusReceiveBuffer.remove(length + 2);  // remove unwanted message from buffer and start over
        ibusState = FIND_SOURCE;
        return;
      }
      for (int i = 0; i <= length + 1; i++) {  // read message from buffer
        ibusByte[i] = ibusReceiveBuffer.read();
      }
#ifdef IBUS_DEBUG
      printDebugMessage("Good Message -> ");  // debug print good message
#endif
      busInSync = true;
      compareIbusPacket();
      ibusState = FIND_SOURCE;
      break;

    case BAD_CHECKSUM:
#ifdef IBUS_DEBUG
      printDebugMessage("Message Bad -> ");  // debug print bad message
#endif
      ibusReceiveBuffer.remove(1);  // remove first byte and start over
      ibusState = FIND_SOURCE;
      break;

  }  // end of switch
}  // end of readIbus

#ifdef IBUS_DEBUG
void BMW_IBus_KBus::printDebugMessage(const char *debugPrefix) {
  if (IbusDebug == NULL) {
    return;
  }
  IbusDebug->print(debugPrefix);
  for (int i = 0; i <= length + 1; i++) {
    if (ibusByte[i] < 0x10) {
      IbusDebug->print(F("0"));
    }
    IbusDebug->print(ibusByte[i], HEX);
    IbusDebug->print(F(" "));
  }
  IbusDebug->println();
}
#endif

void BMW_IBus_KBus::compareIbusPacket() {
  byte *pData = &ibusByte[0];
  if (pIbusPacketHandler != NULL) {
    pIbusPacketHandler((byte *)pData);
  }
}

void BMW_IBus_KBus::write(const byte message[], byte size, bool addChecksum) {
  byte checksum = 0;

  ibusSendBuffer.write(addChecksum ? size + 1 : size);
  for (int i = 0; i < size; i++) {
    byte dataByte = pgm_read_byte(&message[i]);  // message is stored in PROGMEM
    checksum ^= dataByte;                        // Calculate checksum
    ibusSendBuffer.write(dataByte);
  }
  if (addChecksum) {  // skipped when the message already ends with its checksum
    ibusSendBuffer.write(checksum);
  }
}

void BMW_IBus_KBus::sendIbusMessageIfAvailable() {
#if !defined(IBUS_USE_TIMER2)
  updateClearToSend();
#endif
  if (clearToSend && ibusSendBuffer.available() > 0) {  // clearToSend &&
    if (millis() - packetTimer >= packetGap) {
      sendIbusPacket();
      packetTimer = millis();
    }
  }
}

void BMW_IBus_KBus::sendIbusPacket() {
  int Length = ibusSendBuffer.read();
  if (digitalRead(senStaPin) == LOW && Length <= 32) {  // digitalRead(senStaPin) == LOW &&
#if defined(IBUS_DEBUG)
    if (IbusDebug != NULL) IbusDebug->print(F("TRANSMITING CODE: "));
#endif
    for (int i = 0; i < Length; i++) {
      byte dataByte = ibusSendBuffer.read();
      Ibus_Serial->write(dataByte);  // write byte to IBUS.
#if defined(IBUS_DEBUG)
      if (IbusDebug != NULL) {
        if (dataByte < 0x10) IbusDebug->print(F("0"));
        IbusDebug->print(dataByte, HEX);
        IbusDebug->print(F(" "));
      }
#endif
    }
#if defined(IBUS_DEBUG)
    if (IbusDebug != NULL) {
      IbusDebug->println();
      IbusDebug->println();
    }
#endif
  } else {
    ibusSendBuffer.remove(Length);
    return;
  }
}

void BMW_IBus_KBus::sleepEnable(unsigned long sleepTime) {
  ibusTimeToSleep = sleepTime * 1000;
  sleepEnabled = true;
}

void BMW_IBus_KBus::sleep() {
  if (sleepEnabled == true) {
    if (millis() - ibusSleepTime >= ibusTimeToSleep) {
      digitalWrite(enablePin, LOW);  // Shutdown TH3122
    }
  }
}

byte BMW_IBus_KBus::calculateChecksum(const byte *data, byte length) {
  byte checksum = 0;
  for (byte i = 0; i < length; i++) {
    checksum ^= data[i];
  }
  return checksum;
}

void BMW_IBus_KBus::run() {
  if (Ibus_Serial == NULL) {  // setIbusSerial() has not been called yet
    return;
  }
  readIbus();
  sendIbusMessageIfAvailable();
  sleep();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(IBUS_USE_TIMER2)

void BMW_IBus_KBus::contentionTimer() {
#define START_TIMER2 TCCR2B |= (1 << CS22) | (1 << CS21)  // //Set CS22 and CS21 bits for 256 prescaler
#define STOP_TIMER2 TCCR2B &= 0B11111000

  // initialize Timer1
  TCCR2A = 0;  // set entire TCCR2A register to 0
  TCCR2B = 0;  // same for TCCR2B

  // set compare match register to desired timer count:
  OCR2A = 94;  // Set timer to fire CTC interrupt after approx 1.5ms

  // turn on CTC mode:
  TCCR2B |= (1 << WGM21);

  // enable timer compare interrupt:
  TIMSK2 |= (1 << OCIE2A);
  pinMode(senStaPin, INPUT);
}

//========================

void BMW_IBus_KBus::startTimer() {
  if (digitalRead(senStaPin) == LOW) {
    START_TIMER2;
  } else if (digitalRead(senStaPin) == HIGH) {
    clearToSend = false;
    digitalWrite(ledPin, HIGH);
    STOP_TIMER2;
  }
}

void BMW_IBus_KBus::updateClearToSend() {
}

//========================

ISR(TIMER2_COMPA_vect) {
  clearToSend = true;
  digitalWrite(ledPin, LOW);
  STOP_TIMER2;
}

#else

void BMW_IBus_KBus::contentionTimer() {
}

//========================

void IBUS_ISR_ATTR BMW_IBus_KBus::startTimer() {
  if (digitalRead(senStaPin) == LOW) {
    busIdleSince = micros();
    busIdle = true;
  } else {
    busIdle = false;
    clearToSend = false;
    digitalWrite(ledPin, HIGH);
  }
}

//========================

void BMW_IBus_KBus::updateClearToSend() {
  boolean nowClear = false;

  noInterrupts();
  if (busIdle && !clearToSend && micros() - busIdleSince >= contentionTime) {
    clearToSend = true;
    nowClear = true;
  }
  interrupts();

  if (nowClear) {
    digitalWrite(ledPin, LOW);
  }
}

#endif
