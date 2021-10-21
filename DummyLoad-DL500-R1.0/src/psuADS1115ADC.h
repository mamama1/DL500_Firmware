#ifndef psuads1115adc_h
	#define psuads1115adc_h

	#include <Wire.h>
	#include <dnxADS1015.h>
	#include <config.h>

	#define DEFAULT_ADC_POLL_INTERVAL 			500

	class psuADS1115ADC
	{
		public:
			psuADS1115ADC(dnxADS1115 *const ADC_ADS1115);

			void init(uint16_t vref, uint16_t interval, uint8_t averaging, uint8_t adcDivider, bool ch01Diff, bool ch12Diff);

			bool process();

			void readRawData(int& chA, int& chB);
			void readVoltageBoth(int& chA, int& chB);
			int readVoltageA(float multiplier = 1);
			int readVoltageB(float multiplier = 1);

			int readRawDataSingleEnded(uint8_t channel);
			int readVoltageSingleEnded(uint8_t channel);			

		private:
			dnxADS1115 *_adc_ads1115;
			
			// Settings
			uint16_t _pollInterval;
			uint16_t _vref;
			uint8_t _averaging;
			uint8_t _adcDivider;

			// State machine
			bool _immediatePoll;
			unsigned long _lastPollMillis;
			uint8_t _pollIndex;
			
			// Status variables
			int _channelA[ADC_AVERAGING];
			int _channelB[ADC_AVERAGING];
			int _channel0SingleEnded[ADC_AVERAGING];
			int _channel1SingleEnded[ADC_AVERAGING];
			int _channel2SingleEnded[ADC_AVERAGING];
			int _channel3SingleEnded[ADC_AVERAGING];

			int _channelASummation;
			int _channelBSummation;
			int _channel0SummationSingleEnded;
			int _channel1SummationSingleEnded;
			int _channel2SummationSingleEnded;
			int _channel3SummationSingleEnded;

			float _channelAAverage;
			float _channelBAverage;
			float _channel0AverageSingleEnded;
			float _channel1AverageSingleEnded;
			float _channel2AverageSingleEnded;
			float _channel3AverageSingleEnded;

			bool _ch01Diff;
			bool _ch12Diff;

			bool _firstAveragingCompleted;
	};
#endif