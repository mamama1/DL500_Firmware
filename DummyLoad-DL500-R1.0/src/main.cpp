#include <Arduino.h>
// #include <EEPROM.h>
#include <pins.h>
#include <config.h>

#include <LiquidCrystal.h>
#include <dnxDAC70501.h>
#include <dnxADS1015.h>

#include <DisplayFunctions.h>
#include <Bounce2.h>
#include <Encoder.h>

#include "psuADS1115ADC.h"
#include "psuDAC.h"

#include <DLComms.h>
#include <DLDisplay.h>
// #include <DLEnc.h>
#include <ButtonDebouncer.h>
#include <CRC32.h>


dnxADS1115 ads1115(0x48);		// 1001000; ADDR PIN LOW;  CHA: VIN;			CH2: ASHUNT
dnxDAC70501 DAC(0x4A);

psuADS1115ADC adc(&ads1115);				// 1001000; ADDR PIN LOW;  CHA: V_ADJ;			CH2: TH0
psuDAC dac(&DAC);

DLComms comm(Serial1);
DLDisplay dldisplay;


ButtonDebouncer ButtonEncoder;

// uint8_t initIndicator[9] = {'d', 'n', 'x', 'p', 's', 'u', 'd', 'l', '5'};

// state machine

uint16_t milliAmpsSetVal = 0;
uint16_t milliAmpsReadVal = 0;
uint16_t voltsReadVal = 0;
uint32_t powerReadVal = 0;

uint32_t lastUARTsendMillis = 0;

// prototypes
void initPins();
void processADC();
void processUART();

void setup()
{
	// initDAC(VOLTAGE_LOWER_LIMIT, CURRENT_UPPER_LIMIT);
	
	Serial.begin(115200);
	Serial1.begin(28800, SERIAL_8N1);
	
	initPins();
	dldisplay.Init(DLDisplay::DL_DISPLAY::VALUES, &milliAmpsSetVal, &milliAmpsReadVal, &voltsReadVal, &powerReadVal, DISPLAY_UPDATE_INVERVAL);
	

	dldisplay.OnEncoderUp([]() {
		if (milliAmpsSetVal + 1 <= CURRENT_UPPER_LIMIT)
		{
			dldisplay.EncoderUp();
			milliAmpsSetVal += 1;
			dac.set(milliAmpsSetVal);
		}
		else
		{
			milliAmpsSetVal = CURRENT_UPPER_LIMIT;
		}
		dldisplay.Refresh();
	});

	dldisplay.OnEncoderDown([]() {
		if (milliAmpsSetVal >= CURRENT_LOWER_LIMIT + 1)
		{
			dldisplay.EncoderDown();
			milliAmpsSetVal -= 1;
			dac.set(milliAmpsSetVal);
		}
		else
		{
			milliAmpsSetVal = CURRENT_LOWER_LIMIT;
		}
		dldisplay.Refresh();
	});

	ButtonEncoder.Begin(NULL, 	 BDButtonState::PRESSED);
	ButtonEncoder.OnButtonPress([](void *, uint8_t buttonID) {
		Serial.println("ButtonEncoder_OnButtonPress!");
		dldisplay.ButtonPressed();
	});

	// initDisplay();
	// drawHomeScaffold();
	adc.init(ADC_VREF, ADC_POLL_INTERVAL, ADC_AVERAGING, ADC_INPUT_VOLTAGE_DIVIDER, true, true);
	dac.init(DAC_MIN_SET_DELAY, DAC_LOWER_LIMIT, DAC_UPPER_LIMIT, 0, DAC_GAIN);
}

void loop()
{
	// processButtons();
	ButtonEncoder.Process(0, 	!digitalRead(ENC_BTN_PIN));
	processADC();
	dac.process();
	dldisplay.Process();
	processUART();
}

void initPins()
{
	pinMode(ENC_BTN_PIN, INPUT_PULLUP);
}

void processUART()
{
	// RX
	if (Serial1.available())
	{

	}

	// TX
	if ((millis() - lastUARTsendMillis > UART_SEND_INTERVAL))
	{
		comm.sendData(milliAmpsSetVal, milliAmpsReadVal, voltsReadVal, powerReadVal);
		lastUARTsendMillis = millis();
	}
}

void processADC()
{
	if (adc.process())			// ADC values have updated
	{
		voltsReadVal = adc.readVoltageA(26.66);
		milliAmpsReadVal = adc.readVoltageB();
		powerReadVal = ((uint32_t)voltsReadVal * (uint32_t)milliAmpsReadVal) / 1000;
	}
}






// void loadSettings()
// {
// 	// load init indicator from EEPROM
// 	uint8_t bufCompareInitIndicator[9];
// 	EEPROM.get(EEPROM_ADDR_INIT_INDICATOR, bufCompareInitIndicator);
	
// 	// load settings if init indicator matches, otherwise save default settings and init indicator to EEPROM
// 	if (memcmp(bufCompareInitIndicator, initIndicator, sizeof(bufCompareInitIndicator)) == 0)
// 	{
// 		EEPROM.get(EEPROM_ADDR_CALIBRATION, calibrationValues);
// 	}
// 	else
// 	{
// 		EEPROM.put(EEPROM_ADDR_CALIBRATION, calibrationValues);
// 		EEPROM.put(EEPROM_ADDR_INIT_INDICATOR, initIndicator);
// 	}	
// }

