#pragma once

#include <Arduino.h>

// typedef enum class MessageVAlignment : uint8_t
// {
// 	TOP = 		0U,
// 	MIDDLE =	1U,
// 	BOTTOM =	2U
// } MessageVAlignment_t;


// bool forceDisplayUpdate = false;

// char messageLines[LCD_ROWS][LCD_COLS + 1] = {0, };

// extern LiquidCrystal LCD;

// extern uint16_t adcVoltageReadAverage;
// extern uint16_t adcCurrentReadAverage;

// extern uint16_t adcVoltageReadAverage_prev;
// extern uint16_t adcCurrentReadAverage_prev;

// // extern bool adcVoltageReadAverageChanged;
// // extern bool adcCurrentReadAverageChanged;
// // extern bool adcOvervoltageAverageChanged;

// extern uint16_t displayVoltageRead;
// extern uint16_t displayCurrentRead;

// extern uint16_t lastRawDACval;

// void initDisplay()
// {
// 	LCD.begin(LCD_COLS, LCD_ROWS);

// 	LCD.createChar(0, symCL);
// 	LCD.createChar(1, symCR);
// 	LCD.createChar(2, symVR);
// 	LCD.createChar(3, symSettings);
// 	LCD.createChar(4, symFull);
// }

// void drawHomeScaffold()
// {
// 	LCD.setCursor(0, 0);
// 	LCD.print("     SET|   READ|   "); LCD.setCursor(0, 1);
// 	LCD.print("> 0.000A| 0.000V|   "); LCD.setCursor(0, 2);
// 	LCD.print("        | 0.000A|   "); LCD.setCursor(0, 3);
// 	LCD.print("        | 0.000W|   ");

// 	LCD.setCursor(2, 0);
// 	LCD.write(byte(3));
// }

// void displayMainScreen()
// {
// 	drawHomeScaffold();
// }