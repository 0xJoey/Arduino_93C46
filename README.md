# 93C46 Library for Arduino

 **N.b. This library only works for chips in the 64x16 mode. If your chip support 128x8 mode, please make sure your ORG pin is tied to GND.**
 128x8 support is planned in the future, i.e., when I get a chip that supports it.

[93C46 Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/doc5140.pdf)

## Introduction

The 93C46 is a 1KB EEPROM communicating over 3-wire serial, with an additional CS wire. As I could not find any ready-made solution, I decided to write this library. This library sends data to the chip by bitbanging, so it's not going to give the best performance.

## EW (Erase/Write)

When the chip is powered on, EW is disabled. This blocks are attempts to erase/write anything on the chip. Once enabled, EW will stay enabled until it is explicitly disabled or the chip loses power.

## Organization

Some 93C46 chips support different organizations. If the ORG pin is pulled high, the chip with organize itself in 64 words of 16 bits, if ORG is pulled low, it will organize itself as 128 words of 8 bits. The mode the library uses can be changed with `set_mode`. The library starts with 16-bit mode by default.

Data written into one organization may or may not be able to be read in the other organization. Your mileage may vary, have fun.

Using the mode not corresponding to the chip organization will result in undefined behaviour.

## Methods

| Method | Returns | EW required | Description |
| ------ | ------ | ------ | ------ |
| `eeprom_93C46(int pCS, int pSK, int pDI, int pDO)` | - | - | Constructor |
| `set_mode(bool longMode)` | `void` | No | `true` enables 16-bit mode, `false` enables 8-bit mode |
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
 * (For some chips:) GND/5V to pin 6 (This determines the organization, 5V is 16-bit, GND is 8-bit)
 * 5V to Chip pin 8
 * 
 */
#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

// Prints all words of the buffer
void debugPrint(word* buff, int len) {
  Serial.print("\n\t00\t01\t02\t03\t04\t05\t06\t07\t08\t09\t0A\t0B\t0C\t0D\t0E\t0F");
  for(int i = 0; i < len; i++) {
    if(i % 16 == 0) {
      Serial.println();
      Serial.print(i, HEX);
    }
    Serial.print("\t");
    if(buff[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(buff[i], HEX);
  }
}

void setup() {
  bool longMode = true; // Change this to 'false' to use the 8-bit mode
  
  eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);
  e.set_mode(longMode);
  Serial.begin(9600);

  Serial.println("Writing data...");
  // First, enable EW (Erase/Write)
  e.ew_enable();

  String writeBuffer;
  if(longMode) {
    writeBuffer = "This is a string written in the 16-bit organization.\nHi, world!\0";
  } else {
    writeBuffer = "This is a string written in the 8-bit organization.\nAs you can see, the address space for this mode is much bigger!\nHey, world!\0";
  }

  int len = longMode ? 64 : 128;
  // Write your data
  for(int i = 0; i < len; i++) {
    e.write(i, writeBuffer[i]);
  }

  // Optionally, disable EW after writing
  e.ew_disable();

  Serial.println("Reading data...\n");
  word readBuffer[len];
  for(int i = 0; i < len; i++) {
    word r = e.read(i);
    readBuffer[i] = r;
    Serial.print(char(r));
  }
  debugPrint(readBuffer, len);
  Serial.println();
  delay(500);
}

void loop() {}
```