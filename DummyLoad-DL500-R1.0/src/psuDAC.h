#ifndef psudac_h
	#define psudac_h

	#include <config.h>
	#include <dnxDAC70501.h>

	#define DEFAULT_DAC_SET_DELAY 			0

	// #define DACDEBUG

	class psuDAC
	{
		public:
			psuDAC(dnxDAC70501 *const DAC_DAC70501);

			void init(uint16_t minDelay, uint16_t minVal, uint16_t maxVal, uint16_t startVal, float controlLoopGain);

			bool increase(uint16_t multiplier = 1);
			bool decrease(uint16_t multiplier = 1);

			bool set(uint16_t val);
			uint16_t get();

			bool process();

			uint16_t calcDACValue(uint16_t val);

		private:
			dnxDAC70501 *_dac_dac70501;
			
			// Settings
			uint16_t _minDelay;;
			uint16_t _minVal;
			uint16_t _maxVal;
			float _controlLoopGain;
			//uint16_t _chStartup[2];

			// State machine
			unsigned long _lastSetMillis;
			uint16_t _currentVal;
			uint16_t _dacValue;
			bool _dacUpdated;
			
			// Status variables
	};

#endif