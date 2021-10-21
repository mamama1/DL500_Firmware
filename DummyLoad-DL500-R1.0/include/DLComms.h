#pragma once
#include <Arduino.h>

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