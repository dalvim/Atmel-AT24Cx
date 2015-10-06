/*
Documentation here is for method explanation only, not API description
*/

#include <Wire.h>
#include "AT24Cx.h"


// CONSTRUCTORS

AT24Cx::AT24Cx(uint8_t deviceAddress, uint16_t capacity, uint8_t pageSize, uint8_t writeCycle) :
		_deviceAddress(deviceAddress),
		_capacity(capacity),
		_pageSize(pageSize),
		_writeCycle(writeCycle) {

	Wire.begin();
}


// SELECTORS


AT24Cx::ReturnCode AT24Cx::Read(uint16_t startingAddress, void* data, uint16_t length) const {
	Wire.beginTransmission(_deviceAddress);
	Wire.write((uint8_t)(startingAddress >> 8)); // high order byte
	Wire.write((uint8_t)(startingAddress & 0xFF)); // low order byte
	ReturnCode rt = ReturnCode(Wire.endTransmission()); // send & keep connected! TODO: TESTAR se a true eh indiferente...
	if (rt != SUCCESS) { // error
		return rt;
	}

	uint8_t *byteData = (uint8_t*) data; // byte by byte
	uint8_t bytesToRead = 0;
	boolean lastRead = false;
	for (uint16_t i=0; i < length; i++) {
		if (i % WIRE_BUFFER_SIZE == 0) {  // first iteration; filled buffer -> clear buffer.
			bytesToRead = min(length-i, WIRE_BUFFER_SIZE);
			if (length-i <= WIRE_BUFFER_SIZE) {
				lastRead = true;
			}

			if (Wire.requestFrom(_deviceAddress, bytesToRead, lastRead) == 0) {
				return NO_BYTES_AVAILABLE ;
			}
		}
		byteData[i] = Wire.read();
	}

	return SUCCESS;
}


AT24Cx::ReturnCode AT24Cx::Poll(uint16_t memoryAddress) const {
	ReturnCode rt = DEFAULT_VALUE;
	for (uint8_t i = 0; i < _MAX_POLL_ATTEMPTS; i++) {
		delay(_writeCycle);                          // give it a moment
		Wire.beginTransmission(_deviceAddress);
		Wire.write((uint8_t)(memoryAddress >> 8));   // high order byte
		Wire.write((uint8_t)(memoryAddress & 0xFF)); // low-order byte
		rt = ReturnCode(Wire.endTransmission());     // The first 4 are synced with endTransmission() return values.
		if (rt == SUCCESS) {
			break;
		}
	}

	return rt;
}


uint16_t AT24Cx::Capacity() const  {
	return _capacity;
}

uint8_t AT24Cx::PageSize() const  {
	return _pageSize;
}

AT24Cx::ReturnCode AT24Cx::Print(uint16_t startingAddress, uint16_t length, const char* info, uint8_t chunkSize) const {
	AT24Cx::ReturnCode rt = DEFAULT_VALUE;
	uint16_t currentAddress = startingAddress;
	uint8_t _chunkSize = !chunkSize ? _pageSize : chunkSize ;

	uint16_t remainingBytes = length;
	uint16_t bytesToRead = min(remainingBytes, _chunkSize);
	uint8_t data[bytesToRead];

	if (info != 0) {
		Serial.println(info);
	}
	while (remainingBytes > 0) {
		rt = Read(currentAddress, data, bytesToRead);
		if (rt != SUCCESS) {
			break;
		}
		for (uint16_t i = 0; i < bytesToRead; i++) {
			Serial.print(String(data[i]) + " ");
		}
		Serial.print('\n');

		currentAddress += bytesToRead;
		remainingBytes -= bytesToRead;
		bytesToRead = min(remainingBytes, _chunkSize);

	}
	return rt;
}

// MODIFIERS


/**
* Wire.h buffer is 32. With the memory address bytes we can only write 30 bytes at a time.
* Writes in the EEPROM wrap at the end of _pageSize. So we need to keep things aligned.
*/
AT24Cx::ReturnCode AT24Cx::Write(uint16_t startingAddress, const void* data, uint16_t length) const  {
	uint8_t *byteData = (uint8_t *) data; // byte by byte
	uint16_t currentAddress = startingAddress ;
	uint8_t bytesToWrite = 0;
	ReturnCode rt = DEFAULT_VALUE;

	uint16_t endingAddress = min(startingAddress + length, Capacity());

	while (currentAddress < endingAddress) {
		Wire.beginTransmission(_deviceAddress);
		Wire.write((uint8_t)(currentAddress >> 8));   // high order byte
		Wire.write((uint8_t)(currentAddress & 0xFF)); // low order byte

		bytesToWrite = min(
			min(_pageSize - (currentAddress % _pageSize), WIRE_BUFFER_SIZE - 2), // align to page; two bytes lost to address on beginTransmission()
			length - (currentAddress - startingAddress)                            // in case of last bytes.
		);

		Wire.write( &byteData[currentAddress-startingAddress], bytesToWrite) ;

		rt = ReturnCode(Wire.endTransmission()); // send & terminate connection
		if (rt != SUCCESS) { // error
			break;
		}
		currentAddress += bytesToWrite;

		// Wait until all bytes written...
		rt = ReturnCode(Poll(startingAddress));
		if (rt != SUCCESS) { // error
			break;
		}
	}

	return rt ;
}


/**
* Fill() - Fills length bytes with byteValue starting in startingAddress.
*
*/
AT24Cx::ReturnCode AT24Cx::Fill(uint8_t byteValue, uint16_t startingAddress, uint16_t length) const {
	AT24Cx::ReturnCode rt = DEFAULT_VALUE;
	uint16_t currentAddress = startingAddress;

	uint16_t remainingBytes = length;
	uint16_t chunkSize = min(remainingBytes, _pageSize);
	uint8_t data[chunkSize] ;

	for (uint16_t i = 0; i < chunkSize; i++) {
		data[i] = byteValue;
	}

	while (remainingBytes > 0) {
		rt = Write(currentAddress, data, chunkSize);

		if (rt != SUCCESS) {
			break;
		}
		currentAddress += chunkSize;
		remainingBytes -= chunkSize;
		chunkSize = min(remainingBytes, _pageSize);
	}
	return rt;
}

AT24Cx::ReturnCode AT24Cx::Fill(uint8_t byte, uint16_t length) const {
	return Fill(byte, 0, length);
}

AT24Cx::ReturnCode AT24Cx::Fill(uint8_t byte) const {
	return Fill(byte, _capacity);
}

AT24Cx::ReturnCode AT24Cx::Clear(uint16_t startingAddress, uint16_t length) const {
	return Fill(0, startingAddress, length);
}


AT24Cx::ReturnCode AT24Cx::Clear(uint16_t length) const {
	return Clear(0, length);
}

AT24Cx::ReturnCode AT24Cx::Clear() const {
	return Clear(0, _capacity);
}
