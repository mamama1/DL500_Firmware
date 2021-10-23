#pragma once

#include <Arduino.h>

#include <LiquidCrystal.h>
#include <Encoder.h>

#include <config.h>
#include <pins.h>


class DLDisplay
{
	public:

		typedef enum DL_DISPLAY : uint8_t
		{
			NONE =						0x00,
			VALUES =					0x10,
			EDIT_E =					0x11,
			EDIT_Z =					0x12,
			EDIT_H =					0x13,
			EDIT_T =					0x14,
			CONFIG =					0x20
		} DLDisplay_t;		

		DLDisplay();

		void Init(DLDisplay_t initialState, uint16_t *currentSet, uint16_t *currentRead, uint16_t *voltageRead, uint32_t *powerRead, uint16_t displayUpdateIntervalMillis);
		void Process();
		void SetState(DLDisplay_t state);
		void Refresh();
		void ButtonPressed();
		void EncoderUp();
		void EncoderDown();

		void OnEncoderUp(void (*func)());
		void OnEncoderDown(void (*func)());

	private:

		LiquidCrystal LCD;//(LCD_RS_PIN, LCD_EN_PIN, LCD_DB0_PIN, LCD_DB1_PIN, LCD_DB2_PIN, LCD_DB3_PIN,LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN);
		Encoder enc;//(ENC_B_PIN, ENC_A_PIN);

		DLDisplay_t _currentState = DL_DISPLAY::NONE;

		uint16_t *_currentSet;
		uint16_t *_currentRead;
		uint16_t *_voltageRead;
		uint32_t *_powerRead;

		void _drawValuesScaffold();
		void _drawSettingsScaffold();
		void _displayValue(uint16_t val);

		void _processDisplay();
		void _processEncoder();

		uint8_t _cursorBlinkPosX = 0;
		uint8_t _cursorBlinkPosY = 0;
		bool _cursorBlink = false;

		uint32_t _lastDisplayUpdateMillis = 0;
		uint16_t _displayUpdateIntervalMillis = 0;

		bool _forceDisplayUpdate = false;

		// Encoder stuff
		//
		long _encoderOldPosition = -999;

		void (*_EncoderUpFunction)();
		void (*_EncoderDownFunction)();

};