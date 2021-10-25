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
uint16_t milliAmpsSetValDisplay = 0;
uint16_t milliAmpsReadVal = 0;
uint16_t voltsReadVal = 0;
uint32_t powerReadVal = 0;

uint8_t IP[4] = {0, 0, 0, 0};
uint8_t NM[4] = {0, 0, 0, 0};
uint8_t GW[4] = {0, 0, 0, 0};

char SSID[32] = 	"DL500";
char WPA2PSK[32] = 	"DL500PSK";

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

	// while(!Serial.available()){};
	// Serial.println("Hello world");

	initPins();
	dldisplay.Init(DLDisplay::DL_DISPLAY::VALUES1, &milliAmpsSetVal, &milliAmpsSetValDisplay, &milliAmpsReadVal, &voltsReadVal, &powerReadVal, DISPLAY_UPDATE_INVERVAL);
	dldisplay.InitNetworkVals(IP, NM, GW, SSID, WPA2PSK);

	dldisplay.OnEncoderUp([](uint16_t newVal) {
		if (newVal <= CURRENT_UPPER_LIMIT)
		{
			// dldisplay.EncoderUp();
			milliAmpsSetValDisplay = newVal;
			// dac.set(milliAmpsSetVal);
		}
		else
		{
			milliAmpsSetValDisplay = CURRENT_UPPER_LIMIT;
		}
		dldisplay.Refresh();
	});

	dldisplay.OnEncoderDown([](uint16_t newVal) {
		if (newVal >= CURRENT_LOWER_LIMIT && newVal <= CURRENT_UPPER_LIMIT)
		{
			// dldisplay.EncoderDown();
			milliAmpsSetValDisplay = newVal;
			// dac.set(milliAmpsSetVal);
		}
		else
		{
			milliAmpsSetValDisplay = CURRENT_LOWER_LIMIT;
		}
		dldisplay.Refresh();
	});

	dldisplay.OnEncoderConfirmValue([](uint16_t newVal) {
		milliAmpsSetVal = newVal;
		dac.set(milliAmpsSetVal);
		
		// dldisplay.Refresh();
	});

	ButtonEncoder.Begin(NULL, 	 BDButtonState::PRESSED | BDButtonState::HOLD);
	ButtonEncoder.OnButtonPress([](void *, uint8_t buttonID) {
		Serial.println("ButtonEncoder_OnButtonPress!");
		dldisplay.ButtonPressed();
	});

	ButtonEncoder.OnButtonHold([](void *, uint8_t buttonID) {
		Serial.println("ButtonEncoder_OnButtonRepeat!");
		dldisplay.ButtonHold();
	});

	DLDisplay::DLPage_t *ValuesPage = dldisplay.AddPage(	"     SET|   READ|   ",
															"  0.000A| 0.000V|   ",
															"        | 0.000A|   ",
															"        | 0.000W|   ");

	dldisplay.AddPageItem(ValuesPage, &milliAmpsSetVal, 1, 1, true, true);
	dldisplay.AddPageItem(ValuesPage, "Cfg", 2, 4, DLDisplay::DLPAGEENUM::CONFIG, true, false);
	dldisplay.AddPageItem(ValuesPage, &voltsReadVal, 1, 9, false);
	dldisplay.AddPageItem(ValuesPage, &milliAmpsReadVal, 2, 9, false);
	dldisplay.AddPageItem(ValuesPage, &powerReadVal, 3, 9, false);

	dldisplay.SetPage(DLDisplay::DLPAGEENUM::VALUES);


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

