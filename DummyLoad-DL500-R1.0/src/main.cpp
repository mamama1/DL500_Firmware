#include <Arduino.h>
#include <RedirectPrintf.h>
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

// Encoder library hack
// #define CORE_NUM_INTERRUPT	3
// #define CORE_INT0_PIN		10
// #define CORE_INT1_PIN		11
// #define CORE_INT2_PIN		2


dnxADS1115 ads1115(0x48);		// 1001000; ADDR PIN LOW;  CHA: VIN;			CH2: ASHUNT
dnxDAC70501 DAC(0x4A);

psuADS1115ADC adc(&ads1115);				// 1001000; ADDR PIN LOW;  CHA: V_ADJ;			CH2: TH0
psuDAC dac(&DAC);

DLComms comm(Serial1);
DLDisplay dldisplay;


ButtonDebouncer ButtonEncoder;

typedef enum OPMODE : uint8_t
{
	AMPERE =					0x01,
	WATTS =						0x02,
	NOTHING =					0xFF
} OpMode_t;

OpMode_t opMode = OPMODE::AMPERE;

// uint8_t initIndicator[9] = {'d', 'n', 'x', 'p', 's', 'u', 'd', 'l', '5'};

// state machine

uint16_t milliAmpsSetVal = 0;
uint16_t milliAmpsMinVal = 0;
uint16_t milliAmpsMaxVal = 500;

uint16_t milliWattsSetVal = 0;
uint16_t milliWattsMinVal = 0;
uint16_t milliWattsMaxVal = 12500;

uint16_t milliAmpsSetValDisplay = 0;
uint16_t milliAmpsReadVal = 0;
uint16_t voltsReadVal = 0;
uint16_t powerReadVal = 0;

uint32_t wattHourValInternal = 0;
uint16_t wattHourVal = 0;

uint8_t IP[4] = {192, 168, 25, 222};
uint8_t NM[4] = {255, 255, 255, 0};
uint8_t GW[4] = {192, 168, 25, 252};

char SSID[33] = 	"DL500";		// 32 byte max.
char WPA2PSK[64] = 	"DL500PSK";		// 63 byte max.

uint32_t lastUARTsendMillis = 0;

uint32_t heartbeatMillis = 0;
uint32_t wattHourMillis = 0;

uint8_t valuesPage = 0;
uint8_t ampsPageItemId = 0;
uint8_t wattsPageItemId = 0;

// prototypes
void initPins();
void processADC();
void processUART();

void setup()
{
	// initDAC(VOLTAGE_LOWER_LIMIT, CURRENT_UPPER_LIMIT);
	PrintfBegin(&Serial);
	// Serial.begin(115200);
	Serial1.begin(28800, SERIAL_8N1);

	// while(!Serial.available()){};
	// Serial.println("Hello world");

	initPins();
	dldisplay.Init(&milliAmpsSetVal, &milliAmpsSetValDisplay, &milliAmpsReadVal, &voltsReadVal, &powerReadVal, DISPLAY_UPDATE_INVERVAL);
	dldisplay.InitNetworkVals(IP, NM, GW, SSID, WPA2PSK);

	// dldisplay.OnEncoderUp([](uint16_t newVal) {
	// 	if (newVal <= CURRENT_UPPER_LIMIT)
	// 	{
	// 		// dldisplay.EncoderUp();
	// 		milliAmpsSetValDisplay = newVal;
	// 		// dac.set(milliAmpsSetVal);
	// 	}
	// 	else
	// 	{
	// 		milliAmpsSetValDisplay = CURRENT_UPPER_LIMIT;
	// 	}
	// 	dldisplay.Refresh();
	// });

	// dldisplay.OnEncoderDown([](uint16_t newVal) {
	// 	if (newVal >= CURRENT_LOWER_LIMIT && newVal <= CURRENT_UPPER_LIMIT)
	// 	{
	// 		// dldisplay.EncoderDown();
	// 		milliAmpsSetValDisplay = newVal;
	// 		// dac.set(milliAmpsSetVal);
	// 	}
	// 	else
	// 	{
	// 		milliAmpsSetValDisplay = CURRENT_LOWER_LIMIT;
	// 	}
	// 	dldisplay.Refresh();
	// });

	dldisplay.OnEncoderConfirmValue([](void *newVal, DLDisplay::DLItemType_t type) {

		if (type == DLDisplay::DLITEMTYPE::UINT16)
		{
			if (newVal == &milliAmpsSetVal)
			{
				milliAmpsSetVal = *(uint16_t*)newVal;
				dac.set(milliAmpsSetVal);
				// opMode = OPMODE::AMPERE;
			}
			else if (newVal == &milliWattsSetVal)
			{
				milliWattsSetVal = *(uint16_t*)newVal;
				// opMode = LASTSET::WATTS;
				// LOG("new mW: %u\r\n", milliWattsSetVal);
			}
		}
		else if (type == DLDisplay::DLITEMTYPE::STRINGARRAY)
		{
			// LOG("selected item: %s\r\n", (char *)newVal);
			if (strcmp((char*)newVal, "CC") == 0)
			{
				// LOG("CC\r\n");
				opMode = OPMODE::AMPERE;
			}
			else if (strcmp((char*)newVal, "CP") == 0)
			{
				// LOG("CP\r\n");
				opMode = OPMODE::WATTS;
			}
		}		
		// dldisplay.Refresh();
	});

	ButtonEncoder.Begin(NULL, 	 BDButtonState::PRESSED | BDButtonState::HOLD);
	ButtonEncoder.OnButtonPress([](void *, uint8_t buttonID) {
		// Serial.println("ButtonEncoder_OnButtonPress!");
		dldisplay.ButtonPressed();
	});

	ButtonEncoder.OnButtonHold([](void *, uint8_t buttonID) {
		// Serial.println("ButtonEncoder_OnButtonHold!");
		dldisplay.ButtonHold();
	});

	valuesPage = dldisplay.AddPage(	"Mode:",
									"",
									"        |",
									"        |");
	dldisplay.AddPageItem(0, 2, 0, 6, 1, false, "CC", "CP");
	ampsPageItemId = dldisplay.AddPageItem(0, &milliAmpsSetVal, &milliAmpsMinVal, &milliAmpsMaxVal, 10, 2, DLDisplay::DLUNIT::AMPERE, 0, 10, 1, true, true, true);
	wattsPageItemId = dldisplay.AddPageItem(0, &milliWattsSetVal, &milliWattsMinVal, &milliWattsMaxVal, 10, 3, DLDisplay::DLUNIT::WATT, 0, 10, 1, true, true, true);	
	dldisplay.AddPageItem(0, DLDisplay::DLICON::WRENCH, 0, 19, 1, 1, true, false);
	dldisplay.AddPageItem(0, &voltsReadVal, NULL, NULL, 0, 0, DLDisplay::DLUNIT::VOLT, 2, 0, 1, false, false);
	dldisplay.AddPageItem(0, &milliAmpsReadVal, NULL, NULL, 0, 0, DLDisplay::DLUNIT::AMPERE, 3, 0, 1, false, false);
	dldisplay.AddPageItem(0, &powerReadVal, NULL, NULL, 0, 0, DLDisplay::DLUNIT::WATT, 2, 10, 1, false, false);
	dldisplay.AddPageItem(0, &wattHourVal, NULL, NULL, 0, 0, DLDisplay::DLUNIT::WATTHOUR, 3, 10, 1, false, false);

	dldisplay.AddPage(	"",
						"",
						"",
						"");
	dldisplay.AddPageItem(1, "IP Configuration", 0, 1, 1, 2, true, true);
	dldisplay.AddPageItem(1, "WiFi Configuration", 1, 1, 1, 3, true, false);
	dldisplay.AddPageItem(1, "Exit", 2, 1, 1, 0, true, false);
	
	dldisplay.AddPage(	" IP:",
						" NM:",
						" GW:",
						"");
	dldisplay.AddPageItem(2, IP, 0, 5);
	dldisplay.AddPageItem(2, NM, 1, 5);
	dldisplay.AddPageItem(2, GW, 2, 5);
	dldisplay.AddPageItem(2, "Exit", 3, 1, 1, 1, true, true);

	dldisplay.AddPage(	" SSID:",
						" PSK:",
						" State:",
						"");

	dldisplay.AddPageItem(3, SSID, 0, 7, 1, false, false);
	dldisplay.AddPageItem(3, WPA2PSK, 1, 6, 1, false, false);
	dldisplay.AddPageItem(3, "Access Point", 2, 8, 1, 1, false, false);
	dldisplay.AddPageItem(3, "Exit", 3, 1, 1, 1, true, true);
	
	dldisplay.SetPage(0);


	// initDisplay();
	// drawHomeScaffold();
	adc.init(ADC_VREF, ADC_POLL_INTERVAL, ADC_AVERAGING, ADC_INPUT_VOLTAGE_DIVIDER, true, true);
	dac.init(DAC_MIN_SET_DELAY, DAC_LOWER_LIMIT, DAC_UPPER_LIMIT, 0, DAC_GAIN);
}

void loop()
{
	ButtonEncoder.Process(0, 	!digitalRead(ENC_BTN_PIN));
	processADC();
	dac.process();
	dldisplay.Process();
	processUART();

	if (millis() - heartbeatMillis > 1000)
	{
		heartbeatMillis = millis();
		// LOG("Heartbeat\r\n");
		// delay(5);
		
		
	}

	if (millis() - wattHourMillis > 1000)
	{
		wattHourMillis = millis();
		// LOG("Heartbeat\r\n");
		// delay(5);
		wattHourValInternal += (powerReadVal * 100000) / 3600;
		wattHourVal = wattHourValInternal / 100000;
		LOG("wattHourValInternal: %lu\r\nwattHourVal:         %u\r\n", wattHourValInternal, wattHourVal);
	}
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

		if (opMode == OPMODE::WATTS)
		{
			dldisplay.PageItemVisible(valuesPage, ampsPageItemId, false);
			dldisplay.PageItemVisible(valuesPage, wattsPageItemId, true);

			milliAmpsSetVal = round((float)((float)milliWattsSetVal / (float)voltsReadVal) * (float)1000);
			if (milliAmpsSetVal > DAC_UPPER_LIMIT)
			{
				milliAmpsSetVal = DAC_UPPER_LIMIT;
			}

			if (dac.set(milliAmpsSetVal))
			{
				// LOG("mW set: %u, V read: %u, mA set: %u\r\n", milliWattsSetVal, voltsReadVal, milliAmpsSetVal);
			}
		}
		else
		{
			dldisplay.PageItemVisible(valuesPage, ampsPageItemId, true);
			dldisplay.PageItemVisible(valuesPage, wattsPageItemId, false);
		}
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

