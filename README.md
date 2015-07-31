# Atmel-AT24Cx
Arduino Lib for Atmel AT24Cx EEPROMs

Wire.h has a 32 byte buffer. These EEPROMs have an inner page size.
When writing or reading into these EEPROMs you have to have this in mind, but that makes the code less readable.

This lib abstracts from both Wire.h's 32 byte buffer and from the EEPROM's page size.

This means you can read and write whatever you want up to the EEPROM's limit or up to the Arduino's memory capacity.

Please note that the Arduino is very limited in memory: Uno has only 2k SRAM. So trying to allocate an array of 1500+ bytes is not a good idea :) Better do it in chunks. :)


    Copyright (C) <2015>  <Duarte Alvim>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.



## Error Codes: 

    enum ReturnCode { SUCCESS, DATA_TOO_LONG, NACK_ON_ADDRESS, NACK_ON_DATA, OTHER, NO_BYTES_AVAILABLE, DEFAULT_VALUE } ;

## Methods:

### Constructor

		/**
		* AT24Cx - Creates a new instance of an AT24Cx
		*
		* Arguments
		*	deviceAddress: The I2C address of the device
		*	uint16_t capacity: The EEPROM capacity in uint8_ts. (i.e. 32Kbit = 4096 uint8_ts)
		*	writeCycle: The self timed write cycle. The time it takes for a write to be completed.
		*	pageSize: The size of the memory pages in the EEPROM.
		*/
		AT24Cx(uint8_t deviceAddress, uint16_t capacity, uint8_t pageSize, uint8_t writeCycle);

### Read (raw)

		/**
		* Read - Reads data from EEPROM and fills it into 'data'.
		*
		* 'data' is raw data read as bytes. It is of type 'void' so as to avoid the need of cast while passing an argument.
		*
		* If startingAddress + length > Capacity(), it wraps to the begginning.
		*/
		ReturnCode Read(uint16_t startingAddress, void* data, uint16_t length) const ;


### Read (Any type)
		/**
		* Read - Reads data from EEPROM and fills it into 'data'
		*
		* If startingAddress + length > Capacity(), it wraps to the begginning.
		*/
		template <typename T> ReturnCode Read(uint16_t startingAddress, T& data) const {
			return Read(startingAddress, (void*) &data, sizeof(data));
		}

### Poll - Test whether the EEPROM is ready for read/write
		/**
		* Poll - Acknowledge polling: Tests if responds with zero (internal write cycle has ended) 
		* 
		* In case of a NACK (still writing) it continues polling up to _MAX_POLL_ATTEMPTS 
		*/
		ReturnCode Poll(uint16_t memoryAddress) const ;


### Print - Substitutes 'ToString()' because Arduino doesn't have enough memory to create a String the size of the EEPROM.
		/**
		* Prints the contents of the eeprom into 'Serial'.
		* This methods substitutes ToString() since the Arduino probably does not have enough memory
		* to create a string the size of the EEPROM.
		*
		* info is text you may want to show before showing the EEPROM's contents.
		*/
		ReturnCode Print(uint16_t startingAddress, uint16_t length, const char* info = 0) const ;


### Write (Raw)
		/**
		* Write length bytes of the 'data' array into the EEPROM.
		*/
		ReturnCode Write(uint16_t startingAddress, const void* data, uint16_t length) const ;

### Write (Any type)
		/**
		* Writes 'data' into the EEPROM.
		*/
		template <typename T> ReturnCode Write(uint16_t startingAddress, const T& data) const {
			return Write(startingAddress, (const void*) &data, sizeof(data));
		}

### Fill - Fills the EEPROM with given byte. Has several overloaded methods for convenience ;)

		/**
		* Fills the EEPROM with 'byte', starting at 'startAddress' and writing 'length' bytes.
		*/
		ReturnCode Fill(uint8_t byte, uint16_t startingAddress, uint16_t length) const ;


		/**
		* Fills the EEPROM with 'byte', starting at position 0 and writing 'length' bytes.
		*/
		ReturnCode Fill(uint8_t byte, uint16_t length) const ;

		/**
		* Fills the whole EEPROM with 'byte'.
		*/		
		ReturnCode Fill(uint8_t byte) const ;

		/**
		* Zeroes EEPROM with '0', starting at 'startAddress' and writing 'length' bytes.
		*/		
		ReturnCode Clear(uint16_t startingAddress, uint16_t length) const ;
		
		/**
		* Zeroes the EEPROM, starting at position 0 and writing 'length' bytes.
		*/
		ReturnCode Clear(uint16_t length) const ;
		
		/**
		* Zeroes the whole EEPROM.
		*/
		ReturnCode Clear() const; 


		
