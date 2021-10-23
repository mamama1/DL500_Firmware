#pragma once

#include <Arduino.h>
#include <CRC32.h>

#ifdef ARDUINO_ARCH_ESP8266
	#include "SoftwareSerial.h"
#endif

#define RX_TIMEOUT_MILLIS	1000
#define DL_DATA_BUFFER_SIZE 20

class DLComms
{
	public:

		typedef enum DL_COMMAND : uint8_t
		{
			NONE =						0x00,
			DATA =						0x10,	// Measuring data packet
			CURRENT_SET =				0x11,	// Set current
			CURRENT_UP =				0x12,	// Increase current
			CURRENT_DOWN =				0x13,	// Decrease current
			ACK	=						0xFF	// Acknowledge packet
		} DLCommand_t;

		typedef struct __attribute((__packed__)) __attribute__((__may_alias__))
		{
			DLCommand_t		Command = DL_COMMAND::NONE;
			uint8_t			packetNumber;
			bool			ackRequested;
			uint8_t			dataBuffer[DL_DATA_BUFFER_SIZE] = {0, };
			uint32_t		crc = 0;
		} DLPacket_t;

		// typedef struct __attribute((__packed__)) __attribute__((__may_alias__))
		// {
		// 	DLCommand_t		Command = DL_COMMAND::ACK;
		// 	uint8_t			packetNumber;
		// 	uint32_t		crc = 0;
		// } DLAck_t;

		typedef struct __attribute((__packed__)) __attribute__((__may_alias__))
		{
			uint16_t 		currentSet = 0;
			uint16_t 		currentRead = 0;
			uint16_t		voltageRead = 0;
			uint32_t		powerRead = 0;
		} DLData_t;

		typedef struct __attribute((__packed__)) __attribute__((__may_alias__))
		{
			uint16_t			value = 0;
		} DLCurrent_t;

		typedef struct __attribute((__packed__)) __attribute__((__may_alias__))
		{
			uint16_t			refreshIntervalMillis = 0;
		} DLSettings_t;

		#ifdef ARDUINO_ARCH_ESP8266
			DLComms(SoftwareSerial &seriala) :
				Seriala	(seriala)
			{}
		#else
			DLComms(HardwareSerial &seriala) :
				Seriala	(seriala)
			{}
		#endif

		void OnDataReceive(void (*func)(DLData_t *const data));

		void Process();

		void sendData(uint16_t currentSet, uint16_t currentRead, uint16_t voltageRead, uint32_t powerRead, bool requestAck = false);

	private:

		void (*DataReceiveFunction)(DLData_t *const data);

		void sendAck(uint8_t packetNumber);

		uint8_t DataRXbuffer[sizeof(DLComms::DLPacket_t)] = {0, };
		uint8_t DataTXbuffer[sizeof(DLComms::DLPacket_t)] = {0, };

		uint8_t DataPacketBuffer[sizeof(DLComms::DLPacket_t)] = {0, };

		bool rxActive = false;
		uint32_t rxStartMillis = 0;
		uint8_t rxBufferIndex = 0;

		uint32_t lastRXmillis = 0;
		uint32_t crcErrorCounter = 0;
		// bool rxComplete = false;

	protected:
		#ifdef ARDUINO_ARCH_ESP8266
			SoftwareSerial &Seriala;
		#else
			HardwareSerial &Seriala;
		#endif

	
};