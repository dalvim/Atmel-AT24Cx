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
		* Equals - Checks for equality between data and data written in eeprom up to repetition times from startingAddress.
		* Because of memory constraints, when data blocks are repeated patterns instead of allocating all data
		* possibly overflowing memory, we can repeat the check for each block.
		* The simplest case is when we want to check if the eeprom is cleared: filled with zeroes
		*
		* Examples:
		* Check if it's cleared from byte 100 to 150:
		*    Equals(100, 0, 150);
		*
		* Check if there are 3 sequential blocks of data at address 150:
		*    Equals(150, data, 3);
		*
		* startingAddress - From where to verify equality
		* data            - a block of data to test for inequality
		* repetitions     - how many sequencial times will test the data block
		*/
		boolean Equals(uint16_t startingAddress, const void* data, uint16_t dataLength, uint16_t repetitions = 1) const ;

		template <typename T> boolean Equals(uint16_t startingAddress, T& data, uint16_t repetitions = 1) const {
			return Equals(startingAddress, (void*) &data, sizeof(data), repetitions);
		}


		/**
		* Returns the capacity of this EEPROM capacity in uint8_ts. (i.e. 32Kbit = 4096 uint8_ts)
		*/
		uint16_t Capacity() const ;

		/**
		* Returns the pageSize of this EEPROM.
		*/
		uint8_t PageSize() const ;


		/**
		* Prints the contents of the eeprom into 'Serial' chunkSize bytes per line. (if chunkSize == 0)
		* it uses _pageSize as size.
		* This methods substitutes ToString() since the Arduino does not have enough memory
		* to create a string the size of the EEPROM.
		*
		* info is text you may want to show before showing the EEPROM's contents.
		*
		* if (length == 0 ) will go up to Capacity
		*/
		ReturnCode Print(uint16_t startingAddress = 0, uint16_t length = 0, const char* info = 0, uint8_t chunkSize = 0) const ;


		// MODIFIERS

		/**
		* Write length bytes of the 'data' array into the EEPROM.
		*
		* Data that would overflow the EEPROM's capacity is lost.
		*
		* Because Writes are limited in the EEPROM, by default it first checks if Equals(startingAddress, data, length),
		* doing nothing if that's the case.
		*
		* Force version implies it will write without checking first.
		*
		* This method was not overloaded because the template version would become ambiguous:
		*    Write(100, anArrayOfBytes, 20);
		* The compiler cannot distinguish between
		*    Write(uint16_t, const void* data, 20) and
		*    Write(uint16_t, const T& data, 20), passing 20 as force argument... boolean is an uint8_t...
		*/
		ReturnCode Write(uint16_t startingAddress, const void* data, uint16_t length) const ;
		ReturnCode ForceWrite(uint16_t startingAddress, const void* data, uint16_t length) const ;


		/**
		* Writes 'data' into the EEPROM.
		*
		* Data that would overflow the EEPROM's capacity is lost.
		*
		* Because Writes are limited in the EEPROM, by default it first checks if Equals(startingAddress, data, length),
		* doing nothing if that's the case.
		*
		* Force version implies it will write without checking first.
		*
		* This method was not overloaded because the template version would become ambiguous:
		*    Write(100, anArrayOfBytes, 20);
		* The compiler cannot distinguish between
		*    Write(uint16_t, const void* data, 20) and
		*    Write(uint16_t, const T& data, 20), passing 20 as force argument... boolean is an uint8_t...
		*/
		template <typename T> ReturnCode Write(uint16_t startingAddress, const T& data) const {
			return Write(startingAddress, (const void*) &data, sizeof(data));
		}

		template <typename T> ReturnCode ForceWrite(uint16_t startingAddress, const T& data) const {
			return ForceWrite(startingAddress, (const void*) &data, sizeof(data));
		}


		/**
		* Fills the EEPROM with 'byte', starting at 'startAddress' and writing 'length' bytes.
		*
		* Because Writes are limited in the EEPROM, by default it first checks if Equals(startingAddress, byte, length),
		* doing nothing if that's the case.
		*
		* Force implies it will write without checking first.
		*/
		ReturnCode Fill(uint8_t byte, uint16_t startingAddress = 0, uint16_t length = 0, boolean force = false) const ;


		/**
		* Zeroes EEPROM with '0', starting at 'startAddress' and writing 'length' bytes.
		*
		* Because Writes are limited in the EEPROM, by default it first checks if Equals(startingAddress, byte, length),
		* doing nothing if that's the case.
		*
		* Force implies it will write without checking first.
		*
		* if (length == 0 ) will go up to Capacity
		*/
		ReturnCode Clear(uint16_t startingAddress = 0, uint16_t length = 0, boolean force = false) const ;


	private:
		uint8_t _deviceAddress;
		uint16_t _capacity;
		uint8_t _writeCycle;
		uint8_t _pageSize;

		static const uint8_t _MAX_POLL_ATTEMPTS = 10;

		/**
		* Returns the chunk size to test for equality in Equals.
		* It's just a helper method to keep code cleaner ;)
		*/
		uint16_t _ChunkSize(uint16_t dataLength, uint16_t repetitions) const ;

};

#endif
