#include "psuADS1115ADC.h"

psuADS1115ADC::psuADS1115ADC(dnxADS1115 *const ADC_ADS1115)
{
	this->_adc_ads1115 = ADC_ADS1115;
	this->_lastPollMillis = 0;
	this->_immediatePoll = false;
	this->_pollInterval = DEFAULT_ADC_POLL_INTERVAL;
	this->_averaging = 8;
	this->_pollIndex = 0;
}

void psuADS1115ADC::init(uint16_t vref, uint16_t interval, uint8_t averaging, uint8_t adcDivider, bool ch01Diff, bool ch12Diff)
{
	this->_vref = vref;
	this->_pollInterval = interval;
	this->_averaging = averaging;
	this->_adcDivider = adcDivider;
	this->_adc_ads1115->begin();
	this->_adc_ads1115->setGain(GAIN_ONE);
	this->_ch01Diff = ch01Diff;
	this->_ch12Diff = ch12Diff;
	this->_firstAveragingCompleted = false;
}

bool psuADS1115ADC::process()
{
	if (this->_lastPollMillis + _pollInterval < millis() || _immediatePoll)
	{
		uint8_t _averagingSanitized = 0;
		if (_firstAveragingCompleted)
		{
			_averagingSanitized = _averaging;
		}
		else
		{
			_averagingSanitized = _pollIndex + 1;
		}
		
		if (_ch01Diff)
		{
			// Read channel 1 and channel 2 in differential mode
			_channelASummation = _channelASummation - _channelA[_pollIndex];
			_channelA[_pollIndex] = this->_adc_ads1115->readADC_Differential_0_1();
			_channelASummation = _channelASummation + _channelA[_pollIndex];
			_channelAAverage = _channelASummation / _averagingSanitized;
		}
		else
		{
			// Read channel 1 and channel 2 in single-ended mode
			_channel0SummationSingleEnded = _channel0SummationSingleEnded - _channel0SingleEnded[_pollIndex];
			_channel0SingleEnded[_pollIndex] = this->_adc_ads1115->readADC_SingleEnded(0);
			_channel0SummationSingleEnded = _channel0SummationSingleEnded + _channel0SingleEnded[_pollIndex];
			_channel0AverageSingleEnded = _channel0SummationSingleEnded / _averagingSanitized;

			_channel1SummationSingleEnded = _channel1SummationSingleEnded - _channel1SingleEnded[_pollIndex];
			_channel1SingleEnded[_pollIndex] = this->_adc_ads1115->readADC_SingleEnded(1);
			_channel1SummationSingleEnded = _channel1SummationSingleEnded + _channel1SingleEnded[_pollIndex];
			_channel1AverageSingleEnded = _channel1SummationSingleEnded / _averagingSanitized;
		}
		

		if (_ch12Diff)
		{
			// Read channel 3 and channel 4 in differential mode
			_channelBSummation = _channelBSummation - _channelB[_pollIndex];
			_channelB[_pollIndex] = this->_adc_ads1115->readADC_Differential_2_3();
			_channelBSummation = _channelBSummation + _channelB[_pollIndex];
			_channelBAverage = _channelBSummation / _averagingSanitized;
		}
		else
		{
			// Read channel 3 and channel 4 in single-ended mode
			_channel2SummationSingleEnded = _channel2SummationSingleEnded - _channel2SingleEnded[_pollIndex];
			_channel2SingleEnded[_pollIndex] = this->_adc_ads1115->readADC_SingleEnded(2);
			_channel2SummationSingleEnded = _channel2SummationSingleEnded + _channel2SingleEnded[_pollIndex];
			_channel2AverageSingleEnded = _channel2SummationSingleEnded / _averagingSanitized;

			_channel3SummationSingleEnded = _channel3SummationSingleEnded - _channel3SingleEnded[_pollIndex];
			_channel3SingleEnded[_pollIndex] = this->_adc_ads1115->readADC_SingleEnded(3);
			_channel3SummationSingleEnded = _channel3SummationSingleEnded + _channel3SingleEnded[_pollIndex];
			_channel3AverageSingleEnded = _channel3SummationSingleEnded / _averagingSanitized;
		}
		
		
		// Serial.print("#");
		// Serial.print(this->_adc_ads1115->readADC_Differential_0_1());
		// Serial.print("#");
		// _channelA[_pollIndex] = this->_adc_ads1115->readADC_SingleEnded(0);
		// _channelB[_pollIndex] = this->_adc_ads1115->readADC_SingleEnded(1);
		
		
		
		

		_pollIndex += 1;
		if (_pollIndex >= _averaging)
		{
			_firstAveragingCompleted = true;
			_pollIndex = 0;
		}

		
		

		_lastPollMillis = millis();
		return true;
	}
	return false;
}

void psuADS1115ADC::readRawData(int& chA, int& chB)
{
	if (_ch01Diff)
	{
		chA = _channelAAverage;
	}
	else
	{
		chA = 0;
	}
	
	if (_ch12Diff)
	{
		chB = _channelBAverage;
	}
	else
	{
		chB = 0;
	}
	
	
}

void psuADS1115ADC::readVoltageBoth(int& chA, int& chB)
{
	if (_ch01Diff)
	{
		chA = this->readVoltageA();
	}
	else
	{
		chA = 0;
	}

	if (_ch12Diff)
	{
		chB = this->readVoltageB();
	}
	else
	{
		chB = 0;
	}
	
    
}

int psuADS1115ADC::readVoltageA(float multiplier)
{
	int out = 0;
	
	if (_ch01Diff)
	{
		out = _channelAAverage * (float)_vref/(float)32768 * (float)multiplier * this->_adcDivider;
	}
	
	// Serial.print("a:");
	// Serial.print(test);
	// Serial.print(";");
	return out;
}

int psuADS1115ADC::readVoltageB(float multiplier)
{
	int out = 0;
	
	if (_ch12Diff)
	{
		out = _channelBAverage * (float)_vref/(float)32768 * (float)multiplier * this->_adcDivider;
	}
	
	// Serial.print("b:");
	// Serial.print(test);
	// Serial.print(";");
	return out;
}

int psuADS1115ADC::readRawDataSingleEnded(uint8_t channel)
{
	if (channel == 0 && _ch01Diff == false)
	{
		// Serial.println(_channel0AverageSingleEnded);
		return _channel0AverageSingleEnded;
	}
	else if (channel == 1 && _ch01Diff == false)
	{
		return _channel1AverageSingleEnded;
	}
	else if (channel == 2 && _ch12Diff == false)
	{
		return _channel2AverageSingleEnded;
	}
	else if (channel == 3 && _ch12Diff == false)
	{
		return _channel3AverageSingleEnded;
	}
	else
	{
		return 0;
	}
}

int psuADS1115ADC::readVoltageSingleEnded(uint8_t channel)
{
	int out = 0;
	if (((channel == 0 || channel == 1) && _ch01Diff == false) || ((channel == 1 || channel == 2) && _ch12Diff == false))
	{
		out = this->readRawDataSingleEnded(channel) * (float)_vref/(float)32768 * this->_adcDivider;
	}
	
	// Serial.print("a:");
	// Serial.print(test);
	// Serial.print(";");
	return out;
}
