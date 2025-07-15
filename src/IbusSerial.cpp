#include "IbusSerial.h"

const byte senStaPin = 3;
const byte enablePin = 4;
const byte ledPin = 13;
volatile boolean clearToSend = false;
unsigned long ibusTimeToSleep;
boolean sleepEnabled;
unsigned long ibusSleepTime = millis();
unsigned long packetTimer = millis();
RingBuffer ibusReceiveBuffer(128);
RingBuffer ibusSendBuffer(64);
#if defined(ESP32)
hw_timer_t *contentionHwTimer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define START_TIMER2 TCCR2B |= (1 << CS22) | (1 << CS21)  // Set CS22 and CS21 bits for 256 prescaler
#define STOP_TIMER2 TCCR2B &= 0B11111000                  // Clear CS22, CS21 and CS20 bits to stop timer
#endif

IbusSerial::IbusSerial()  // constructor
{
  busInSync = false;
  ibusSleepTime = millis();
  packetTimer = millis();
  source = 0;
  length = 0;
  destination = 0;
  ibusState = FIND_SOURCE;
  sleepEnabled = false;
  pinMode(ledPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  contentionTimer();
}

IbusSerial::~IbusSerial()  // deconstructor
{
}

void IbusSerial::setIbusSerial(HardwareSerial &newIbusSerial) {
  Ibus_Serial = &newIbusSerial;
  Ibus_Serial->begin(9600, SERIAL_8E1);  // ibus always 9600 8E1
}

#if defined(IBUS_DEBUG)
void IbusSerial::setIbusDebug(Stream &newIbusDebug) {
  IbusDebug = &newIbusDebug;
}
#endif

void IbusSerial::setIbusPacketHandler(IbusPacketHandler_t newHandler) {
  pIbusPacketHandler = newHandler;
}

void IbusSerial::readIbus() {
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
          // remove source byte and start over
          ibusReceiveBuffer.remove(1);
        }
      }
      break;

    case FIND_MESSAGE:
      if (ibusReceiveBuffer.available() >= length + 2)  // Check if enough bytes in buffer to complete message (based on length byte)
      {
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
      if (source != 0x50     // MFL
          && source != 0x68  // RAD
          && source != 0x18  // CDC
          && source != 0x6A  // DSP
          && source != 0xC8  // TEL
          && source != 0xB0  // SES
          && source != 0xE8  // RLS
          && source != 0x44  // SES
          && source != 0x00  // RLS
      ) {
        // remove unwanted message from buffer and start over
#ifdef IBUS_DEBUG
        IbusDebug->print(F("DISCARDED: "));
        for (int i = 0; i <= length + 1; i++) {
          if (ibusReceiveBuffer.peek(i) < 0x10) {
            IbusDebug->print(F("0"));
          }
          IbusDebug->print(ibusReceiveBuffer.peek(i), HEX);
          IbusDebug->print(F(" "));
        }
        IbusDebug->println();
#endif
        ibusReceiveBuffer.remove(length + 2);
        ibusState = FIND_SOURCE;
        return;
      }
      // read message from buffer
      for (int i = 0; i <= length + 1; i++) {
        ibusByte[i] = ibusReceiveBuffer.read();
      }
#ifdef IBUS_DEBUG
      // debug print good message
      printDebugMessage("Good Message -> ");
#endif
      busInSync = true;
      compareIbusPacket();
      ibusState = FIND_SOURCE;
      break;

    case BAD_CHECKSUM:
#ifdef IBUS_DEBUG
      // debug print bad message
      printDebugMessage("Message Bad -> ");
#endif
      // remove first byte and start over
      ibusReceiveBuffer.remove(1);
      ibusState = FIND_SOURCE;
      break;

  }  // end of switch
}  // end of readIbus

#ifdef IBUS_DEBUG
void IbusSerial::printDebugMessage(const char *debugPrefix) {
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

void IbusSerial::compareIbusPacket() {
  byte *pData = &ibusByte[0];
  pIbusPacketHandler((byte *)pData);
}

void IbusSerial::write(const byte message[], byte size) {
  ibusSendBuffer.write(size);
  for (int i = 0; i < size; i++) {
    ibusSendBuffer.write(pgm_read_byte(&message[i]));
  }
}

void IbusSerial::sendIbusMessageIfAvailable() {
  if (clearToSend && ibusSendBuffer.available() > 0)  // clearToSend &&
  {
    if (millis() - packetTimer >= packetGap) {
      sendIbusPacket();
      packetTimer = millis();
    }
  }
}

void IbusSerial::sendIbusPacket() {
  int Length = ibusSendBuffer.read();
  if (digitalRead(senStaPin) == LOW && Length <= 32)  // digitalRead(senStaPin) == LOW &&
  {
#if defined(IBUS_DEBUG)
    IbusDebug->println(F("TRANSMITING CODE"));
#endif
    for (int i = 0; i < Length; i++) {
      byte dataByte = ibusSendBuffer.read();
      Ibus_Serial->write(dataByte);  // write byte to IBUS.
    }
  } else {
    ibusSendBuffer.remove(Length);
    return;
  }
}

void IbusSerial::sleepEnable(unsigned long sleepTime) {
  ibusTimeToSleep = sleepTime * 1000;
  sleepEnabled = true;
}

void IbusSerial::sleep() {
  if (sleepEnabled == true) {
    if (millis() - ibusSleepTime >= ibusTimeToSleep) {
      digitalWrite(enablePin, LOW);  // Shutdown TH3122
    }
  }
}

void IbusSerial::run() {
  readIbus();
  sendIbusMessageIfAvailable();
  sleep();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(ESP32)
void IRAM_ATTR onContentionTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  clearToSend = true;
  digitalWrite(ledPin, LOW);
  timerStop(contentionHwTimer);
  portEXIT_CRITICAL_ISR(&timerMux);
}

#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
ISR(TIMER2_COMPA_vect) {
  clearToSend = true;
  digitalWrite(ledPin, LOW);
  STOP_TIMER2;
}
#endif

void IbusSerial::contentionTimer() {
  pinMode(senStaPin, INPUT);
  pinMode(ledPin, OUTPUT);

#if defined(ESP32)
  // ESP32 timer setup
  contentionHwTimer = timerBegin(80);                           // 80 prescaler (1 tick = 12.5us)
  timerAttachInterrupt(contentionHwTimer, &onContentionTimer);  // Attach the interrupt handler
  timerWrite(contentionHwTimer, 1500);                          // 1.5ms
  timerStop(contentionHwTimer);                                 // Initially stop

#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
  // AVR timer setup
  TCCR2A = 0;
  TCCR2B = 0;
  OCR2A = 94;               // Set compare value for 1.5ms at 16MHz with 256 prescaler
  TCCR2B |= (1 << WGM21);   // CTC mode
  TIMSK2 |= (1 << OCIE2A);  // Interrupt enable
#endif
}

void IbusSerial::startTimer() {
  if (digitalRead(senStaPin) == LOW) {
#if defined(ESP32)
    timerWrite(contentionHwTimer, 0);  // Reset timer
    timerStart(contentionHwTimer);
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
    START_TIMER2;
#endif

  } else if (digitalRead(senStaPin) == HIGH) {
    clearToSend = false;
    digitalWrite(ledPin, HIGH);

#if defined(ESP32)
    timerStop(contentionHwTimer);
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
    STOP_TIMER2;
#endif
  }
}