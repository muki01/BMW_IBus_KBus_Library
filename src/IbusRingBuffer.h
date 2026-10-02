#include "Arduino.h"

#ifndef IbusRingBuffer_h
#define IbusRingBuffer_h

class IbusRingBuffer {
 private:
  int bufferSize;
  unsigned int bufferHead, bufferTail;
  byte *buffer_p;

 public:
  IbusRingBuffer(int size);
  ~IbusRingBuffer();
  int available(void);
  int peek(void);
  int peek(int);
  void remove(int);
  int read(void);
  byte write(int);
};
#endif
