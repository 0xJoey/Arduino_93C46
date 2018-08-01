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
  
  int len = longMode ? 64 : 128;
  word readBuffer[len];
  for(int i = 0; i < len; i++) {
    // Read by address
    word r = e.read(i);
    readBuffer[i] = r;
    Serial.print(char(r));
  }
  debugPrint(readBuffer, len);
  Serial.println();
}

void loop() {}