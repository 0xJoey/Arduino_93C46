# 93C46 Library for Arduino

 **N.b. This library only works for chips in the 64x16 mode. If your chip support 128x8 mode, please make sure your ORG pin is tied to GND.**
 128x8 support is planned in the future, i.e., when I get a chip that supports it.

[93C46 Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/doc5140.pdf)

## Introduction

The 93C46 is a 1KB EEPROM communicating over 3-wire serial, with an additional CS wire. As I could not find any ready-made solution, I decided to write this library. This library sends data to the chip by bitbanging, so it's not going to give the best performance.

## EW (Erase/Write)

When the chip is powered on, EW is disabled. This blocks are attempts to erase/write anything on the chip. Once enabled, EW will stay enabled until it is explicitly disabled or the chip loses power.

## Methods

| Method | Returns | EW required | Description |
| ------ | ------ | ------ | ------ |
| `eeprom_93C46(int pCS, int pSK, int pDI, int pDO)` | - | - | Constructor |
| `ew_enable()` | `void` | No | Enables EW, disabled read-only mode |
| `ew_disable()` | `void` | Yes | Disables  EW, returns to read-only mode |
| `is_ew_enabled()` | `bool` | No | `true` if EW is enabled |
| `erase_all()` | `void` | Yes | Changes all memory locations to `0xFFFF` |
| `write_all(word value)` | `void` | Yes | Changes all memory locations to the provided value |
| `write(byte addr, word value)` | `void` | Yes | Changes the provided memory location to the provided value |
| `erase(byte addr)` | `void` | Yes | Changes the provided memory location to `0xFDFF`|
| `read(byte addr)` | `word` | No | Returns the value of the provided memory location|

# Example

```
#include <93C46.h>
/*
 * Example Sketch demonstration on how to write to a 93C46 eeprom
 * 
 * Wiring:
 * Pin 7(CS) to Chip pin 1
 * Pin 9(CS) to Chip pin 2
 * Pin 10(DI/MOSI) to Chip pin 3
 * Pin 11(DO/MISO) to Chip pin 4
 * GND to Chip pin 5
 * (For some chips:) GND to pin 6
 * 5V to Chip pin 8
 * 
 */
#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

// Prints all 64 words of the buffer
void debugPrint(word* buff, int len) {
  Serial.print("\n\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\tA\tB\tC\tD\tE\tF");
  for(int i = 0; i < 64; i++) {
    if(i % 16 == 0) {
      Serial.println();
      Serial.print(i, HEX);
    }
    Serial.print("\t");
    Serial.print(buff[i], HEX);
  }
}

void setup() {
  // put your setup code here, to run once:
  eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);
  Serial.begin(9600);

  Serial.println("Writing data...");
  // First, enable EW (Erase/Write)
  e.ew_enable();

  char writeBuffer[64] = "Hello world, this is a test string! abcdefghijklmnopqrstuvwxyz.\0";

  // Write your data
  for(int i = 0; i < 64; i++) {
    e.write(i, writeBuffer[i]);
  }

  // Optionally, disable EW after writing
  e.ew_disable();


  Serial.println("Reading data...");
  word readBuffer[64];
  for(int i = 0; i < 64; i++) {
    word r = e.read(i);
    readBuffer[i] = r;
    Serial.print(char(r));
  }
  debugPrint(readBuffer, 64);
  Serial.println();
  delay(500);
}

void loop() {}
```