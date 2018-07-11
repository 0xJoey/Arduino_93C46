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
