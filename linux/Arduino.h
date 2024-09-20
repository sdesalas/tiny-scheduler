#ifndef __LINUX_ARDUINO__
#define __LINUX_ARDUINO__

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned char byte;
typedef unsigned char uint8_t;

static const uint8_t SS   = 10;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 12;
static const uint8_t SCK  = 13;

static const uint8_t SDA = 18;
static const uint8_t SCL = 19;
#define LED_BUILTIN 13

static const uint8_t A0 = 14;
static const uint8_t A1 = 15;
static const uint8_t A2 = 16;
static const uint8_t A3 = 17;
static const uint8_t A4 = 18;
static const uint8_t A5 = 19;
static const uint8_t A6 = 20;
static const uint8_t A7 = 21;

const byte INPUT        = 'z';
const byte INPUT_PULLUP = 'Z';
const byte OUTPUT       = 'o';
const byte HIGH         = 1;
const byte LOW          = 0;

void delay(unsigned long ms);

void delayMicroseconds(unsigned int us);

unsigned long pulseIn(uint8_t pin, byte value);

unsigned long pulseIn(uint8_t pin, byte value, unsigned long timeout);

unsigned long millis();

unsigned long micros();

void pinMode(byte pin, byte mode);

void digitalWrite(byte pin, byte value);

byte digitalRead(byte pin);

unsigned short analogRead(byte pin);

void analogWrite(byte pin, byte value);

void pinMode(byte pin, byte mode);

void digitalWrite(byte pin, byte value);

byte digitalRead(byte pin);

unsigned short analogRead(byte pin);

void analogWrite(byte pin, byte value);

const int DEC = 10;
const int HEX = 16;
const int OCT = 8;
const int BIN = 2;

class Print;

class Printable {
public:
  virtual size_t printTo(Print& p) const = 0;
};

class Print {
private:
  int write_error;
  size_t printNumber(unsigned long, uint8_t);
  size_t printFloat(double, uint8_t);

protected:
  void setWriteError(int err = 1) { write_error = err; }

public:
  Print() : write_error(0) {}

  int getWriteError() { return write_error; }
  void clearWriteError() { setWriteError(0); }

  virtual size_t write(uint8_t) = 0;
  size_t write(const char* str) {
    if (str == NULL) {
      return 0;
    }
    return write((const uint8_t*) str, strlen(str));
  }
  virtual size_t write(const uint8_t* buffer, size_t size);
  size_t write(const char* buffer, size_t size) { return write((const uint8_t*) buffer, size); }

  size_t print(const char[]);
  size_t print(char);
  size_t print(unsigned char, int = DEC);
  size_t print(int, int = DEC);
  size_t print(unsigned int, int = DEC);
  size_t print(long, int = DEC);
  size_t print(unsigned long, int = DEC);
  size_t print(double, int = 2);
  size_t print(const Printable&);

  size_t println(const char[]);
  size_t println(char);
  size_t println(unsigned char, int = DEC);
  size_t println(int, int = DEC);
  size_t println(unsigned int, int = DEC);
  size_t println(long, int = DEC);
  size_t println(unsigned long, int = DEC);
  size_t println(double, int = 2);
  size_t println(const Printable&);
  size_t println(void);
};

const int PARSE_TIMEOUT = 1000; // default number of milli-seconds to wait
const int NO_SKIP_CHAR  = 1;    // a magic char not found in a valid ASCII numeric field

class Stream : public Print {
protected:
  unsigned long _timeout;     // number of milliseconds to wait for the next char before aborting timed read
  unsigned long _startMillis; // used for timeout measurement
  int timedRead();            // private method to read stream with timeout
  int timedPeek();            // private method to peek stream with timeout
  int peekNextDigit();        // returns the next numeric digit in the stream or -1 if timeout

public:
  virtual int available() = 0;
  virtual int read()      = 0;
  virtual int peek()      = 0;
  virtual void flush()    = 0;

  Stream() { _timeout = 1000; }

  // parsing methods

  void setTimeout(unsigned long timeout); // sets maximum milliseconds to wait for stream data, default is 1 second

  bool find(char* target); // reads data from the stream until the target string is found
  bool find(uint8_t* target) { return find((char*) target); }
  // returns true if target string is found, false if timed out (see setTimeout)

  bool find(char* target, size_t length); // reads data from the stream until the target string of given length is found
  bool find(uint8_t* target, size_t length) { return find((char*) target, length); }
  // returns true if target string is found, false if timed out

  bool findUntil(char* target, char* terminator); // as find but search ends if the terminator string is found
  bool findUntil(uint8_t* target, char* terminator) { return findUntil((char*) target, terminator); }

  bool findUntil(char* target, size_t targetLen, char* terminate, size_t termLen); // as above but search ends if the terminate string is found
  bool findUntil(uint8_t* target, size_t targetLen, char* terminate, size_t termLen) { return findUntil((char*) target, targetLen, terminate, termLen); }

  long parseInt(); // returns the first valid (long) integer value from the current position.
  // initial characters that are not digits (or the minus sign) are skipped
  // integer is terminated by the first character that is not a digit.

  float parseFloat(); // float version of parseInt

  size_t readBytes(char* buffer, size_t length); // read chars from stream into buffer
  size_t readBytes(uint8_t* buffer, size_t length) { return readBytes((char*) buffer, length); }
  // terminates if length characters have been read or timeout (see setTimeout)
  // returns the number of characters placed in the buffer (0 means no valid data found)

  size_t readBytesUntil(char terminator, char* buffer, size_t length); // as readBytes with terminator character
  size_t readBytesUntil(char terminator, uint8_t* buffer, size_t length) { return readBytesUntil(terminator, (char*) buffer, length); }
  // terminates if length characters have been read, timeout, or if the terminator character  detected
  // returns the number of characters placed in the buffer (0 means no valid data found)

  // Arduino String functions to be added here

protected:
  long parseInt(char skipChar); // as above but the given skipChar is ignored
  // as above but the given skipChar is ignored
  // this allows format characters (typically commas) in values to be ignored

  float parseFloat(char skipChar); // as above but the given skipChar is ignored

  struct MultiTarget {
    const char* str; // string you're searching for
    size_t len;      // length of string you're searching for
    size_t index;    // index used by the search routine.
  };

  // This allows you to search for an arbitrary number of strings.
  // Returns index of the target that is found first or -1 if timeout occurs.
  int findMulti(struct MultiTarget* targets, int tCount);
};

class LinuxSerial : public Stream {
public:
  void begin(int bd){};
  int available() { return 0; };
  int read() { return -1; };
  int peek() { return -1; };
  void flush(){};
  size_t write(uint8_t c) {
    char serial_buffer[256];
    sprintf(serial_buffer, "%c", c);
    printf(serial_buffer);
    return strlen(serial_buffer);
  };
};

extern LinuxSerial Serial;

template <typename V> V min(V a, V b) {
  if (a <= b) {
    return a;
  }
  else {
    return b;
  }
}

template <typename V> V max(V a, V b) {
  if (a >= b) {
    return a;
  }
  else {
    return b;
  }
}

long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif