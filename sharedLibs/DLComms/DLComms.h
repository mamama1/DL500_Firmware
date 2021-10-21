#pragma once

#include <Arduino.h>
#include <CRC32.h>

#ifdef ARDUINO_ARCH_ESP8266
	#include "SoftwareSerial.h"
#endif

#define RX_TIMEOUT_MILLIS	1000

class DLComms
{
	public:

		typedef enum class DL_COMMAND : uint8_t
		{
			DATA =						'D',	// Measuring data packet
			CURRENT_SET =				'S',	// Set current
			CURRENT_UP =				'U',	// Increase current
			CURRENT_DOWN =				'O',	// Decrease current
		} DLCommand_t;

		typedef struct __attribute((__packed__)) __attribute__((__may_alias__))
		{
			DLCommand_t		Command = DL_COMMAND::DATA;
			uint16_t 		currentSet = 0;
			uint16_t 		currentRead = 0;
			uint16_t		voltageRead = 0;
			uint32_t		CRC = 0;
		} DLData_t;

		#ifdef ARDUINO_ARCH_ESP8266
			DLComms(SoftwareSerial &seriala) :
				Seriala	(seriala)
			{}
		#else
			DLComms(HardwareSerial &seriala);
				Seriala	(seriala)
			{}
		#endif

		void OnDataReceive(void (*func)(DLData_t *const data));

		void Process();

	private:

		void (*DataReceiveFunction)(DLData_t *const data);

		uint8_t DataRXbuffer[sizeof(DLComms::DLData_t)] = {0, };
		uint8_t DataPacket[sizeof(DLComms::DLData_t)] = {0, };

		bool rxActive = false;
		uint32_t rxStartMillis = 0;
		uint8_t rxBufferIndex = 0;

		uint32_t lastRXmillis = 0;
		uint32_t crcErrorCounter = 0;
		bool rxComplete = false;

	protected:
		#ifdef ARDUINO_ARCH_ESP8266
			SoftwareSerial &Seriala;
		#else
			HardwareSerial &Seriala;
		#endif

	
};