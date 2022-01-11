/*
  93C46.h - Library for the 93C46 EEPROM chip in 64x16 mode
  Created by Joey Haas, Juli 7, 2018
  Released under the GNU General Public License v3.0
*/
#include "Arduino.h"
#include "93C46.h"

enum OP { // Operations
	WRITE 		= 0x40,
	READ 		= 0x80, 
	ERASE 		= 0xC0
};
enum CC { // Control Codes
	EW_DISABLE 	= 0x00, 
	WRITE_ALL 	= 0x10, 
	ERASE_ALL 	= 0x20, 
	EW_ENABLE 	= 0x30
};

eeprom_93C46::eeprom_93C46(int pCS, int pSK, int pDI, int pDO) {
	pinMode(pCS, OUTPUT);
	pinMode(pSK, OUTPUT);
	pinMode(pDI, OUTPUT); 
	pinMode(pDO, INPUT);
	_pCS = pCS;
	_pSK = pSK;
	_pDI = pDI;
	_pDO = pDO;
	_ew = false;
	_mode = true;
};

void eeprom_93C46::set_mode(bool longMode) {
	_mode = longMode;
}

void eeprom_93C46::ew_enable() {
	digitalWrite(_pCS, HIGH);
	send_bits(HIGH, 1);
	if(_mode == EEPROM_93C46_MODE_16BIT) {
		send_bits(EW_ENABLE, 8);
	} else {
		send_bits(EW_ENABLE<<1, 9);
	}
	digitalWrite(_pCS, LOW);
	_ew = true;
};

void eeprom_93C46::ew_disable() {
	digitalWrite(_pCS, HIGH);
	send_bits(HIGH, 1);
	if(_mode == EEPROM_93C46_MODE_16BIT) {
		send_bits(EW_DISABLE, 8);
	} else {
		send_bits(EW_DISABLE<<1, 9);
	}
	digitalWrite(_pCS, LOW);
	_ew = false;
}

bool eeprom_93C46::is_ew_enabled() {
	return _ew;
}

void eeprom_93C46::erase_all() {
	if(!this->is_ew_enabled()) {
		return;
	}
	digitalWrite(_pCS, HIGH);
	send_bits(HIGH, 1);
	if(_mode == EEPROM_93C46_MODE_16BIT) {
		send_bits(ERASE_ALL, 8);
	} else {
		send_bits(ERASE_ALL<<1, 9);
	}
	digitalWrite(_pCS, LOW);
	wait();
}

void eeprom_93C46::write_all(word value) {
	if(!this->is_ew_enabled()) {
		return;
	}
	digitalWrite(_pCS, HIGH);
	send_bits(HIGH, 1);
	if(_mode == EEPROM_93C46_MODE_16BIT) {
		send_bits(WRITE_ALL, 8);
		send_bits(0xFFFF & value, 16);
	} else {
		send_bits(WRITE_ALL<<1, 9);
		send_bits(0xFF & value, 8);
	}
	digitalWrite(_pCS, LOW);
	wait();
}

void eeprom_93C46::write(byte addr, word value) {
	if(!this->is_ew_enabled()) {
		return;
	}
	digitalWrite(_pCS, HIGH);
	send_bits(HIGH, 1);
	if(_mode == EEPROM_93C46_MODE_16BIT) {
		send_bits(WRITE | (addr & 0x3F), 8);
		send_bits(0xFFFF & value, 16);
	} else {
		send_bits(WRITE<<1 | (addr & 0x7F), 9);
		send_bits(0xFF & value, 8);
	}
	digitalWrite(_pCS, LOW);
	wait();
}

void eeprom_93C46::erase(byte addr) {
	if(!this->is_ew_enabled()) {
		return;
	}
	digitalWrite(_pCS, HIGH);
	send_bits(HIGH, 1);
	if(_mode == EEPROM_93C46_MODE_16BIT) {
		send_bits(ERASE | (addr & 0x3F), 8);
	} else {
		send_bits(ERASE<<1 | (addr & 0x7F), 9);
	}
	
	digitalWrite(_pCS, LOW);
	wait();
}

word eeprom_93C46::read(byte addr) {
	word val = 0;
	digitalWrite(_pCS, HIGH);
	send_bits(HIGH, 1);
	
	int amtBits;
	if(_mode == EEPROM_93C46_MODE_16BIT) {
		send_bits(READ >> 6, 2);
		send_bits(addr, 8);
		amtBits = 16;
	} else {
		send_bits(READ<<1 | (addr & 0x7F), 9);
		amtBits = 8;
	}
	// Read bits
	for(int i = amtBits; i>0; i--) {
		delayMicroseconds(1);
		digitalWrite(_pSK, HIGH);
		delayMicroseconds(1);
		byte in = digitalRead(_pDO) ? 1 : 0;
		digitalWrite(_pSK, LOW);
		val |= (in << (i-1));
	}
	digitalWrite(_pCS, LOW);
	return val;
}

void eeprom_93C46::send_bits(word value, int len) {
	for(int i = len-1; i>=0; i--)
	{
		bool toSend = (value & 1<<i);
		// Send bit
		digitalWrite(_pDI, toSend);
		delayMicroseconds(1);
		digitalWrite(_pSK, HIGH);
		delayMicroseconds(1);
		digitalWrite(_pSK, LOW);
		digitalWrite(_pDI, LOW);
	}
}

void eeprom_93C46::wait() {
	delayMicroseconds(1);
	//Wait until action is done.
	digitalWrite(_pCS, HIGH);
	while(digitalRead(_pDO) != HIGH) {
		delayMicroseconds(1);
	}
	digitalWrite(_pCS, LOW);
}
