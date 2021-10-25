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
			NONE1 =						0x00,
			VALUES1 =					0x10,
			EDIT_E =					0x11,
			EDIT_Z =					0x12,
			EDIT_H =					0x13,
			EDIT_T =					0x14,
			CONFIG1 =					0x20,
			CONFIG_IP1 =				0x21,
			CONFIG_WIFI1 =				0x22,
			CONFIG_EXIT =				0x23,
			CONFIG_IP_IP =				0x30,
			CONFIG_IP_IP_EDIT_1 = 		0x31,
			CONFIG_IP_IP_EDIT_2 = 		0x32,
			CONFIG_IP_IP_EDIT_3 = 		0x33,
			CONFIG_IP_IP_EDIT_4 = 		0x34,
			CONFIG_IP_NM =				0x35,
			CONFIG_IP_NM_EDIT_1 =		0x36,
			CONFIG_IP_NM_EDIT_2 =		0x37,
			CONFIG_IP_NM_EDIT_3 =		0x38,
			CONFIG_IP_NM_EDIT_4 =		0x39,
			CONFIG_IP_GW =				0x40,
			CONFIG_IP_GW_EDIT_1 =		0x41,
			CONFIG_IP_GW_EDIT_2 =		0x42,
			CONFIG_IP_GW_EDIT_3 =		0x43,
			CONFIG_IP_GW_EDIT_4 =		0x44,
			CONFIG_IP_EXIT =			0x45,
			CONFIG_WIFI_EXIT =			0x50
		} DLDisplay_t;

		typedef enum DLPAGEENUM : uint8_t
		{
			VALUES =					0x00,
			CONFIG =					0x01,
			CONFIG_IP =					0x02,
			CONFIG_WIFI =				0x03,
			NOPAGE = 					0xFF
		} DLPageEnum_t;

		typedef enum DLITEMTYPE : uint8_t
		{
			UINT16 =					0x01,
			UINT32 =					0x02,
			BOOL =						0x03,
			STRING =					0x04,
			IP =						0x05,
			PAGE =						0x06,
			NOTYPE =					0xFF
		} DLItemType_t;

		typedef enum DLITEMACTION : uint8_t
		{
			JUMP =						0x00,
			EDIT =						0x01
		} DLItemAction_t;

		typedef struct
		{
			DLPageEnum_t	page = DLPAGEENUM::NOPAGE;
			void			*valuePtr;
			const char		*text;
			DLItemType_t	type;
			uint8_t			row;
			uint8_t			col;
			DLPageEnum_t	targetPage = DLPAGEENUM::NOPAGE;
			DLItemAction_t	action = DLITEMACTION::EDIT;
			bool			selectable = false;
		} DLPageItem_t;

		typedef struct
		{
			const char		**scaffoldLines[21];
			DLPageItem_t	*pageItems;
			// DLPageItem_t	*pageItemsNew;
			uint8_t			pageItemCount;
			DLPageItem_t	*selectedItem;
		} DLPage_t;
		
		DLDisplay();

		void Init(DLDisplay_t initialState, uint16_t *currentSet, uint16_t *currentSetDisplay, uint16_t *currentRead, uint16_t *voltageRead, uint32_t *powerRead, uint16_t displayUpdateIntervalMillis);
		void InitNetworkVals(uint8_t *iparray, uint8_t *nmarray, uint8_t *gwarray, char *ssid, char *psk);
		void Process();
		void SetState(DLDisplay_t state);
		void SetPage(DLPageEnum_t page);
		DLDisplay_t GetState();
		void Refresh();
		void ButtonPressed();
		void ButtonHold();
		void EncoderUp();
		void EncoderDown();

		void OnEncoderUp(void (*func)(uint16_t newVal));
		void OnEncoderDown(void (*func)(uint16_t newVal));
		void OnEncoderConfirmValue(void (*func)(uint16_t newVal));

		DLPage_t* AddPage(const char *scaffoldLine1, const char *scaffoldLine2, const char *scaffoldLine3, const char *scaffoldLine4);
		DLPageItem_t* AddPageItem(DLPage_t *page, uint16_t *uint16ValPtr, uint8_t row, uint8_t col, bool editable, bool selected = false);
		DLPageItem_t* AddPageItem(DLPage_t *page, uint32_t *uint32ValPtr, uint8_t row, uint8_t col, bool editable, bool selected = false);
		DLPageItem_t* AddPageItem(DLPage_t *page, const char *text, uint8_t row, uint8_t col, DLPageEnum_t targetPage, bool selectable, bool selected = false);

	private:

		LiquidCrystal LCD;//(LCD_RS_PIN, LCD_EN_PIN, LCD_DB0_PIN, LCD_DB1_PIN, LCD_DB2_PIN, LCD_DB3_PIN,LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN);
		Encoder enc;//(ENC_B_PIN, ENC_A_PIN);

		DLDisplay_t _currentState = DL_DISPLAY::NONE1;
		DLPageEnum_t _currentPage = DLPAGEENUM::NOPAGE;

		uint16_t *_currentSet;
		uint16_t *_currentSetDisplay;
		uint16_t *_currentRead;
		uint16_t *_voltageRead;
		uint32_t *_powerRead;

		uint8_t *_IP;
		uint8_t *_NM;
		uint8_t *_GW;

		char *_SSID;
		char *_PSK;

		// void _drawValuesScaffold();
		// void _drawSettingsScaffold();
		// void _drawSettingsIPScaffold();
		// void _drawSettingsWIFIScaffold();

		void _drawPage(DLPageEnum_t page);

		void _drawPageValues(DLPageEnum_t page);

		void _displayValue(uint32_t val);
		void _displayIP(uint8_t *ip);

		void _processDisplay();
		void _processEncoder();

		void _checkNullPtr(void *ptr);

		// DLPageItem_t ValuePage[4];
		// DLPageItem_t ConfigPage[3];
		// DLPageItem_t ConfigIpPage[4];

		DLPage_t *_pages = NULL;
		uint8_t _pageCount = 0;

		DLPageItem_t *_pageItems = NULL;
		uint8_t _pageItemCount = 0;

		uint8_t _cursorBlinkPosX = 0;
		uint8_t _cursorBlinkPosY = 0;
		bool _cursorBlink = false;

		uint32_t _lastDisplayUpdateMillis = 0;
		uint16_t _displayUpdateIntervalMillis = 0;

		bool _forceDisplayUpdate = false;

		uint16_t _getMultiplier();

		// Encoder stuff
		//
		long _encoderOldPosition = -999;

		void (*_EncoderUpFunction)(uint16_t newVal);
		void (*_EncoderDownFunction)(uint16_t newVal);
		void (*_EncoderConfirmValueFunction)(uint16_t newVal);

};