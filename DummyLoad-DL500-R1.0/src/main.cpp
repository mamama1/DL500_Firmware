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
#include <CRC32.h>

LiquidCrystal LCD(LCD_RS_PIN, LCD_EN_PIN, LCD_DB0_PIN, LCD_DB1_PIN, LCD_DB2_PIN, LCD_DB3_PIN,LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN);
Encoder enc(ENC_B_PIN, ENC_A_PIN);
dnxADS1115 ads1115(0x48);		// 1001000; ADDR PIN LOW;  CHA: VIN;			CH2: ASHUNT
dnxDAC70501 DAC(0x4A);
Bounce debouncerButtonEncoder = Bounce();
psuADS1115ADC adc(&ads1115);				// 1001000; ADDR PIN LOW;  CHA: V_ADJ;			CH2: TH0
psuDAC dac(&DAC);

DLComms comm(Serial1);

// uint32_t lastADCupdateMillis = 0;

// uint16_t adcVoltageRead[ADC_AVERAGING] = { 0 };
// uint16_t adcCurrentRead[ADC_AVERAGING] = { 0 };

// uint16_t adcVoltageReadAverage = 0;
// uint16_t adcCurrentReadAverage = 0;
// uint16_t adcOvervoltageAverage = 0;
// uint16_t adcVoltageReadAverage_prev = 0;
// uint16_t adcCurrentReadAverage_prev = 0;

// bool adcVoltageReadAverageChanged = false;
// bool adcCurrentReadAverageChanged = false;

// uint8_t adcSampleIndex = 0;

// uint16_t displayVoltageRead;
// uint16_t displayCurrentRead;

// uint8_t initIndicator[9] = {'d', 'n', 'x', 'p', 's', 'u', 'd', 'l', '5'};

// state machine
long encoderOldPosition = -999;
bool buttonEncoderUpdated = false;

uint16_t milliAmpsSetVal = 0;
uint16_t milliAmpsReadVal = 0;
uint16_t voltsReadVal = 0;

uint32_t lastUARTsendMillis = 0;

uint8_t DataBuffer[11] = {0, };

// prototypes
void initPins();
void updateButtons();
void processEncoder();
void processButtons();
void processADC();
void processDisplay();
void processUART();

void displayValue(uint16_t val);

void setup()
{
	// initDAC(VOLTAGE_LOWER_LIMIT, CURRENT_UPPER_LIMIT);
	
	Serial.begin(115200);
	Serial1.begin(28800, SERIAL_8N1);
	
	initPins();
	initDisplay();
	drawHomeScaffold();
	adc.init(ADC_VREF, ADC_POLL_INTERVAL, ADC_AVERAGING, ADC_INPUT_VOLTAGE_DIVIDER, true, true);
	dac.init(DAC_MIN_SET_DELAY, DAC_LOWER_LIMIT, DAC_UPPER_LIMIT, 0, DAC_GAIN);
}

void loop()
{
	updateButtons();
	processEncoder();
	processButtons();
	processADC();
	dac.process();
	processDisplay();
	processUART();
}

void updateButtons()
{
	buttonEncoderUpdated = debouncerButtonEncoder.update();
}

void processEncoder()
{
	long encoderNewPosition = enc.read();
	if (encoderNewPosition != encoderOldPosition)
	{
		if (encoderNewPosition > encoderOldPosition + 4)
		{
			if (milliAmpsSetVal + 1 <= CURRENT_UPPER_LIMIT)
			{
				milliAmpsSetVal += 1;
				dac.set(milliAmpsSetVal);
			}
			else
			{
				milliAmpsSetVal = CURRENT_UPPER_LIMIT;
			}
			forceDisplayUpdate = true;
			encoderOldPosition = encoderNewPosition;
		}
		else if (encoderNewPosition < encoderOldPosition - 4)
		{
			if (milliAmpsSetVal >= CURRENT_LOWER_LIMIT + 1)
			{
				milliAmpsSetVal -= 1;
				dac.set(milliAmpsSetVal);
			}
			else
			{
				milliAmpsSetVal = CURRENT_LOWER_LIMIT;
			}
			forceDisplayUpdate = true;
			encoderOldPosition = encoderNewPosition;
		}
	}
}

void processButtons()
{
	if (buttonEncoderUpdated && debouncerButtonEncoder.read() == LOW)
	{
		// Button pressed
	}
}

void processDisplay()
{
	if ((millis() - lastDisplayUpdateMillis > DISPLAY_UPDATE_INVERVAL) || forceDisplayUpdate)
	{
		LCD.setCursor(1, 1);
		displayValue(milliAmpsSetVal);

		LCD.setCursor(9, 1);
		displayValue(voltsReadVal);

		LCD.setCursor(9, 2);
		displayValue(milliAmpsReadVal);

		lastDisplayUpdateMillis = millis();
		forceDisplayUpdate = false;
	}
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
		// auto *const currentPacket = reinterpret_cast<DLComms::DLPacket_t *const>(DataBuffer);

		// valuePacket->Command = DLComms::DL_COMMAND::DATA;
		// valuePacket->currentRead = milliAmpsReadVal;
		// valuePacket->currentSet = milliAmpsSetVal;
		// valuePacket->voltageRead = voltsReadVal;
		// valuePacket->CRC = 0;

		// uint32_t checksum = CRC32::calculate(DataBuffer, sizeof(DataBuffer));

		// valuePacket->CRC = checksum;

		// Serial1.write(DataBuffer, sizeof(DataBuffer));



		// for (uint8_t n = 0; n < sizeof(DataBuffer); n++)
		// {
		// 	Serial.print(DataBuffer[n], HEX);
		// 	Serial.print(" ");
		// }
		// Serial.println();

		comm.sendData(milliAmpsSetVal, milliAmpsReadVal, voltsReadVal, 0);

		lastUARTsendMillis = millis();
	}
}

void processADC()
{
	if (adc.process())			// ADC values have updated
	{
		voltsReadVal = adc.readVoltageA(26.66);
		milliAmpsReadVal = adc.readVoltageB();
	}
}


// ####### HELPER FUNCTIONS

void displayValue(uint16_t val)
{
	if (val < 10000)
		LCD.print(' ');

	LCD.print((int)(val / 1000));
	LCD.print('.');

	val %= 1000;

	if (val < 10)
		LCD.print("00");
	else if (val < 100)
		LCD.print("0");
	
	LCD.print(val);

	// LOG("XXX = %u\r\n", val);	
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

void initPins()
{
	pinMode(ENC_BTN_PIN, INPUT_PULLUP);

	debouncerButtonEncoder.interval(DEBOUNCE_INTERVAL);
	debouncerButtonEncoder.attach(ENC_BTN_PIN);

	encoderOldPosition = enc.read();
	// pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
	// pinMode(BUTTON_ENTER_PIN, INPUT_PULLUP);
	// pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
	// pinMode(BUTTON_OUTPUT_PIN, INPUT_PULLUP);
	
	// pinMode(LED_STATUS_PIN, OUTPUT);
	// pinMode(LED_OUTPUT_PIN, OUTPUT);

	// digitalWrite(LED_STATUS_PIN, LOW);
	// digitalWrite(LED_OUTPUT_PIN, LOW);
	
	// pinMode(REL_OUT_PIN, OUTPUT);
	// digitalWrite(REL_OUT_PIN, LOW);
}