#ifndef dnxDAC70501_h
#define dnxDAC70501_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DACx0501_CMD_DAC_DATA		0b00001000
#define DACx0501_CMD_GAIN			0b00000100

#define DACx0501_GAIN_REFDIV_BY_2	0b0000000100000000
#define DACx0501_GAIN_REFDIV_BY_1	0b0000000000000000
#define DACx0501_GAIN_BUFFGAIN_2X	0b0000000000000001
#define DACx0501_GAIN_BUFFGAIN_1X	0b0000000000000000

typedef enum DACGain : uint8_t
{
	GAIN_1X	=	0U,
	GAIN_2X = 	1U
} DACGain_t;

typedef enum DACVREFDivider : uint8_t
{
	DIVIDE_BY_1	=	0U,
	DIVIDE_BY_2 = 	1U
} DACVREFDivider_t;

class dnxDAC70501
{
	public:
		dnxDAC70501(uint8_t i2cAddr);
		bool Set(uint16_t val);
		void Begin();
		void SetGain(DACGain_t gain);
		void SetVREFDivider(DACVREFDivider_t divider);

	private:
		int _i2cAddr;
		uint16_t _gainRegister;

		DACVREFDivider_t _divider;
		DACGain_t _gain;

		bool sendIntValueI2C(uint16_t val);
		void readGainRegister();
};

#endif
