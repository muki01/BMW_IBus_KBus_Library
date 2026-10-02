#include "IbusRingBuffer.h"

IbusRingBuffer::IbusRingBuffer(int size) {
  bufferSize = size;
  bufferTail = 0;
  bufferHead = 0;
  buffer_p = (byte *)malloc(size);
  memset(buffer_p, 0, size);
}

IbusRingBuffer::~IbusRingBuffer() {
  if (buffer_p) free(buffer_p);
}

// public functions

int IbusRingBuffer::available(void) {
  int ByteCount = (bufferSize + bufferHead - bufferTail) % bufferSize;
  return ByteCount;
}

int IbusRingBuffer::read(void) {
  if (bufferHead == bufferTail) {
    return -1;
  } else {
    byte c = buffer_p[bufferTail];
    bufferTail = (bufferTail + 1) % bufferSize;
    if (bufferHead == bufferTail) {
      bufferTail = 0;
      bufferHead = 0;
    }
    return c;
  }
}

byte IbusRingBuffer::write(int c) {
  if ((bufferHead + 1) % bufferSize == bufferTail) {
    return -1;
  }
  buffer_p[bufferHead] = c;
  bufferHead = (bufferHead + 1) % bufferSize;
  return 0;
}

void IbusRingBuffer::remove(int n) {
  if (bufferHead != bufferTail) {
    bufferTail = (bufferTail + n) % bufferSize;
  }
}

int IbusRingBuffer::peek(void) {
  if (bufferHead == bufferTail) {
    return -1;
  } else {
    return buffer_p[bufferTail];
  }
}

int IbusRingBuffer::peek(int n) {
  if (bufferHead == bufferTail) {
    return -1;
  } else {
    return buffer_p[(bufferTail + n) % bufferSize];
  }
}
