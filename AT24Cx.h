/*
Biblioteca para a EEPROM Atmega AT24C32
Copiado de um forum e adaptada para as minhas convencoes. :p
*/

#ifndef AT24Cx_h
#define AT24Cx_h

#include <Arduino.h>

class AT24Cx {
	public:

		// uint8_t _pageSize; // no need. It's always >= WIRE_BUFFER_SIZE...

		const static uint8_t WIRE_BUFFER_SIZE = 32 ; // dunno why this isn't documented or even defined...

		enum ReturnCode { SUCCESS, DATA_TOO_LONG, NACK_ON_ADDRESS, NACK_ON_DATA, OTHER, NO_BYTES_AVAILABLE, DEFAULT_VALUE } ;

		// CONSTRUCTOR(S)

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

		// SELECTORS

		/**
		* Read - Reads data from EEPROM and fills it into 'data'.
		*
		* 'data' is raw data read as bytes. It is of type 'void' so as to avoid the need of cast while passing an argument.
		*
		* If startingAddress + length > Capacity(), it wraps to the begginning.
		*/
		ReturnCode Read(uint16_t startingAddress, void* data, uint16_t length) const ;


		/**
		* Read - Reads data from EEPROM and fills it into 'data'
		*
		* If startingAddress + length > Capacity(), it wraps to the begginning.
		*/
		template <typename T> ReturnCode Read(uint16_t startingAddress, T& data) const {
			return Read(startingAddress, (void*) &data, sizeof(data));
		}

		/**
		* Poll - Acknowledge polling: Tests if responds with zero (internal write cycle has ended)
		*
		* In case of a NACK (still writing) it continues polling up to _MAX_POLL_ATTEMPTS
		*/
		ReturnCode Poll(uint16_t memoryAddress) const ;

		/**
		* Returns the capacity of this EEPROM
		*/
		uint16_t Capacity() const ;

		uint8_t PageSize() const ;


		/**
		* Prints the contents of the eeprom into 'Serial' chunkSize bytes per line. (if chunkSize == 0)
		* it uses _pageSize as size.
		* This methods substitutes ToString() since the Arduino does not have enough memory
		* to create a string the size of the EEPROM.
		*
		* info is text you may want to show before showing the EEPROM's contents.
		*/
		ReturnCode Print(uint16_t startingAddress, uint16_t length, const char* info = 0, uint8_t chunkSize = 0) const ;


		// MODIFIERS

		/**
		* Write length bytes of the 'data' array into the EEPROM.
		*
		* It overlaps to the begginning of the EEPROM.
		*/
		ReturnCode Write(uint16_t startingAddress, const void* data, uint16_t length) const ;


		/**
		* Writes 'data' into the EEPROM.
		*/
		template <typename T> ReturnCode Write(uint16_t startingAddress, const T& data) const {
			return Write(startingAddress, (const void*) &data, sizeof(data));
		}

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


	private:
		uint8_t _deviceAddress;
		uint16_t _capacity;
		uint8_t _writeCycle;
		uint8_t _pageSize;

		static const uint8_t _MAX_POLL_ATTEMPTS = 10;
};

#endif
