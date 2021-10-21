#pragma once

#include <Arduino.h>

byte symCL[8] = {
	B01111,
	B11001,
	B10111,
	B10111,
	B10111,
	B11001,
	B01111,
	B00000
};

byte symCR[8] = {
	B11110,
	B11001,
	B10111,
	B10111,
	B10111,
	B11001,
	B11110,
	B00000
};

byte symVR[8] = {
	B11110,
	B10101,
	B10101,
	B10101,
	B10101,
	B11011,
	B11110,
	B00000
};

byte symSettings[8] = {
  B00000,
  B00010,
  B11101,
  B00010,
  B01000,
  B10111,
  B01000,
  B00000
};

byte symFull[8] = {
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111
};

typedef enum class MessageVAlignment : uint8_t
{
	TOP = 		0U,
	MIDDLE =	1U,
	BOTTOM =	2U
} MessageVAlignment_t;

uint32_t lastDisplayUpdateMillis = 0;
bool forceDisplayUpdate = false;

char messageLines[LCD_ROWS][LCD_COLS + 1] = {0, };

extern LiquidCrystal LCD;

extern uint16_t adcVoltageReadAverage;
extern uint16_t adcCurrentReadAverage;

extern uint16_t adcVoltageReadAverage_prev;
extern uint16_t adcCurrentReadAverage_prev;

// extern bool adcVoltageReadAverageChanged;
// extern bool adcCurrentReadAverageChanged;
// extern bool adcOvervoltageAverageChanged;

extern uint16_t displayVoltageRead;
extern uint16_t displayCurrentRead;

extern uint16_t lastRawDACval;

void initDisplay()
{
	LCD.begin(LCD_COLS, LCD_ROWS);

	LCD.createChar(0, symCL);
	LCD.createChar(1, symCR);
	LCD.createChar(2, symVR);
	LCD.createChar(3, symSettings);
	LCD.createChar(4, symFull);
}

void drawHomeScaffold()
{
	LCD.setCursor(0, 0);
	LCD.print("     SET|   READ|   "); LCD.setCursor(0, 1);
	LCD.print("> 0.000A| 0.000V|   "); LCD.setCursor(0, 2);
	LCD.print("        | 0.000A|   "); LCD.setCursor(0, 3);
	LCD.print("        | 0.000W|   ");

	LCD.setCursor(2, 0);
	LCD.write(byte(3));
}

void displayMainScreen()
{
	drawHomeScaffold();
}