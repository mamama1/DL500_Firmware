#include "psuDAC.h"

psuDAC::psuDAC(dnxDAC70501 *const DAC_DAC70501)
{
	this->_dac_dac70501 = DAC_DAC70501;
	this->_lastSetMillis = 0;
	this->_minDelay = 0;
	this->_dacUpdated = false;
	this->_currentVal = 0;
}

void psuDAC::init(uint16_t minDelay, uint16_t minVal, uint16_t maxVal, uint16_t startVal, float controlLoopGain)
{
	this->_minDelay = minDelay;
	this->_minVal = minVal;
	this->_maxVal = maxVal;
	this->_currentVal = startVal;
	this->_dacUpdated = true;
	this->_controlLoopGain = controlLoopGain;
	this->_dac_dac70501->Begin();
}

bool psuDAC::increase(uint16_t multiplier)
{
	if (_currentVal == _maxVal)
	{
		_dacUpdated = false;
	}
	else if (_currentVal + multiplier <= _maxVal)
	{
		_currentVal += multiplier;
		_dacUpdated = true;
	}
	else
	{
		_currentVal = _maxVal;
		_dacUpdated = true;
	}

	return _dacUpdated;
}

bool psuDAC::decrease(uint16_t multiplier)
{
	if (_currentVal == _minVal)
	{
		_dacUpdated = false;
	}
	else if (_currentVal >= _minVal + multiplier)
	{
		_currentVal -= multiplier;
		_dacUpdated = true;
	}
	else
	{
		_currentVal = _minVal;
		_dacUpdated = true;
	}

	return _dacUpdated;
}

/// Set output voltage in millivolts
/// 
/// @param val mV
bool psuDAC::set(uint16_t val)
{
	if (val >= _minVal && val <= _maxVal && val != _currentVal)
	{
		_currentVal = val;
		_dacUpdated = true;
		return true;
	}
	else return false;
}

uint16_t psuDAC::get()
{
	return _currentVal;
}

bool psuDAC::process()
{
	bool retval = false;

	if (_lastSetMillis + _minDelay < millis())
	{
		if (_dacUpdated == true)
		{
			// Serial.println(_currentVal);
			this->_dac_dac70501->Set(this->calcDACValue(_currentVal));
			_dacUpdated = false;
			retval = true;
		}

		_lastSetMillis = millis();
	}

	return retval;
}

/// Calculate DAC value from mV
/// 
/// @param val mV
uint16_t psuDAC::calcDACValue(uint16_t val)
{
	float bitSize = 0.0;
	uint16_t retVal = 0;

	if (val < DAC_VREF_HALF * this->_controlLoopGain)
	{
		bitSize = (float)DAC_VREF_HALF / (float)DAC_BITS * (float)this->_controlLoopGain;
		this->_dac_dac70501->SetGain(DACGain::GAIN_1X);
		this->_dac_dac70501->SetVREFDivider(DACVREFDivider::DIVIDE_BY_2);
	}
	else if (val < DAC_VREF * this->_controlLoopGain)
	{
		bitSize = (float)DAC_VREF / (float)DAC_BITS * (float)this->_controlLoopGain;
		this->_dac_dac70501->SetGain(DACGain::GAIN_1X);
		this->_dac_dac70501->SetVREFDivider(DACVREFDivider::DIVIDE_BY_1);
	}
	else if (val < DAC_VREF_DOUBLE * this->_controlLoopGain)
	{
		bitSize = (float)DAC_VREF_DOUBLE / (float)DAC_BITS * (float)this->_controlLoopGain;
		this->_dac_dac70501->SetGain(DACGain::GAIN_2X);
		this->_dac_dac70501->SetVREFDivider(DACVREFDivider::DIVIDE_BY_1);
	}
	else
	{
		#ifdef DACDEBUG
		Serial.println("DACDEBUG: out of range");
		#endif

		// out of range
		return 0;
	}

	retVal = constrain(round((float)val / bitSize) - DAC_OFFSET, 0, DAC_BITS - 1);

	#ifdef DACDEBUG
		Serial.print("DACDEBUG: val: ");
		Serial.print(val);
		Serial.print(" bitSize: ");
		Serial.print(bitSize);
		Serial.print(" result: ");
		Serial.println(retVal);
	#endif

	return retVal;
	
}