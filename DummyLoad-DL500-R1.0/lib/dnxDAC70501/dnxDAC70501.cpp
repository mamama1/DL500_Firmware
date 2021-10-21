#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "dnxDAC70501.h"
// #include <SPI.h>
#include <Wire.h>


dnxDAC70501::dnxDAC70501(uint8_t i2cAddr)
{
	// _CS = CS;

	// pinMode(_CS,OUTPUT);
	// digitalWrite(_CS,HIGH);
	// SPI.begin();
	// SPI.setClockDivider(SPI_CLOCK_DIV8);
	Wire.begin();
	_i2cAddr = i2cAddr;
}

//************************************************************************
bool dnxDAC70501::Set(uint16_t val)
{
	return sendIntValueI2C(val);
}

//************************************************************************ 

void dnxDAC70501::Begin()
{
	Wire.begin();
}

bool dnxDAC70501::sendIntValueI2C(uint16_t val)
{
	// int _val = val;
	byte lowVal, highVal;
	val = val << 2;

	lowVal = lowByte(val);
	highVal = highByte(val);

	// Serial.print("DACval (gain: ");
	// Serial.print(_gain);
	// Serial.print(" vrefdiv: ");
	// Serial.print(_divider);
	// Serial.print("): ");
	// Serial.println(val);

	Wire.beginTransmission(_i2cAddr);
	Wire.write(DACx0501_CMD_DAC_DATA);
	Wire.write(highVal);
	Wire.write(lowVal);
	
  	if (Wire.endTransmission() != 0) return false; else return true;
}

void dnxDAC70501::SetGain(DACGain_t gain)
{
	this->readGainRegister();

	if (gain == DACGain::GAIN_1X)
	{
		_gainRegister &= (DACx0501_GAIN_BUFFGAIN_1X | 0b1111111111111110);
	}
	else if (gain == DACGain::GAIN_2X)
	{
		_gainRegister |= DACx0501_GAIN_BUFFGAIN_2X;
	}

	_gain = gain;

	// Serial.println("Result Modified");
	// Serial.print("  Hex: ");
	// Serial.print("0x");
	// Serial.println(_gainRegister, HEX);

	Wire.beginTransmission(_i2cAddr);
	Wire.write(DACx0501_CMD_GAIN);
	Wire.write(highByte(_gainRegister));
	Wire.write(lowByte(_gainRegister));
	
  	Wire.endTransmission();
}

void dnxDAC70501::SetVREFDivider(DACVREFDivider_t divider)
{
	this->readGainRegister();

	if (divider == DACVREFDivider::DIVIDE_BY_1)
	{
		_gainRegister &= (DACx0501_GAIN_REFDIV_BY_1 | 0b1111111011111111);
	}
	else if (divider == DACVREFDivider::DIVIDE_BY_2)
	{
		_gainRegister |= DACx0501_GAIN_REFDIV_BY_2;
	}

	_divider = divider;

	// Serial.println("Result Modified");
	// Serial.print("  Hex: ");
	// Serial.print("0x");
	// Serial.println(_gainRegister, HEX);

	Wire.beginTransmission(_i2cAddr);
	Wire.write(DACx0501_CMD_GAIN);
	Wire.write(highByte(_gainRegister));
	Wire.write(lowByte(_gainRegister));
	
  	Wire.endTransmission();
}

void dnxDAC70501::readGainRegister()
{
	Wire.beginTransmission(_i2cAddr);
	Wire.write(DACx0501_CMD_GAIN);
	Wire.endTransmission(0);
	Wire.requestFrom(_i2cAddr, 2);
	_gainRegister = ((Wire.read() << 8) | Wire.read());

	// Output results
	// Serial.println("Result Read Gain register Hex: 0x");
	// Serial.println(_gainRegister, HEX);
}