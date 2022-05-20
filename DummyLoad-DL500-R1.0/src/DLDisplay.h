#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <RedirectPrintf.h>

#include <config.h>
#include <pins.h>


class DLDisplay
{
	public:

		// typedef enum DL_DISPLAY : uint8_t
		// {
		// 	NONE1 =						0x00,
		// 	VALUES1 =					0x10,
		// 	EDIT_E =					0x11,
		// 	EDIT_Z =					0x12,
		// 	EDIT_H =					0x13,
		// 	EDIT_T =					0x14,
		// 	CONFIG1 =					0x20,
		// 	CONFIG_IP1 =				0x21,
		// 	CONFIG_WIFI1 =				0x22,
		// 	CONFIG_EXIT =				0x23,
		// 	CONFIG_IP_IP =				0x30,
		// 	CONFIG_IP_IP_EDIT_1 = 		0x31,
		// 	CONFIG_IP_IP_EDIT_2 = 		0x32,
		// 	CONFIG_IP_IP_EDIT_3 = 		0x33,
		// 	CONFIG_IP_IP_EDIT_4 = 		0x34,
		// 	CONFIG_IP_NM =				0x35,
		// 	CONFIG_IP_NM_EDIT_1 =		0x36,
		// 	CONFIG_IP_NM_EDIT_2 =		0x37,
		// 	CONFIG_IP_NM_EDIT_3 =		0x38,
		// 	CONFIG_IP_NM_EDIT_4 =		0x39,
		// 	CONFIG_IP_GW =				0x40,
		// 	CONFIG_IP_GW_EDIT_1 =		0x41,
		// 	CONFIG_IP_GW_EDIT_2 =		0x42,
		// 	CONFIG_IP_GW_EDIT_3 =		0x43,
		// 	CONFIG_IP_GW_EDIT_4 =		0x44,
		// 	CONFIG_IP_EXIT =			0x45,
		// 	CONFIG_WIFI_EXIT =			0x50
		// } DLDisplay_t;

		// typedef enum DLPAGEENUM : uint8_t
		// {
		// 	VALUES =					0x00,
		// 	CONFIG =					0x01,
		// 	CONFIG_IP =					0x02,
		// 	CONFIG_WIFI =				0x03,
		// 	NOPAGE = 					0xFF
		// } DLPageEnum_t;

		typedef enum DLITEMTYPE : uint8_t
		{
			UINT16 =					0x01,
			UINT32 =					0x02,
			BOOL =						0x03,
			STRING =					0x04,
			STRINGARRAY =				0x05,
			IP =						0x06,
			PAGE =						0x07,
			ICON =						0x08,
			NOTYPE =					0xFF
		} DLItemType_t;

		typedef enum DLITEMACTION : uint8_t
		{
			JUMP =						0x00,
			EDIT =						0x01,
			NONE =						0xFF
		} DLItemAction_t;

		typedef enum DLUNIT : uint8_t
		{
			NOUNIT =					0x00,
			AMPERE =					0x01,
			VOLT =						0x02,
			WATT =						0x03,
			OHM =						0x04,
			WATTHOUR =					0x05
		} DLUnit_t;

		typedef enum DLICON : uint8_t
		{
			WRENCH =					0x00
		} DLIcon_t;

		typedef struct
		{
			uint16_t		*valuePtr = NULL;
			uint16_t		*maxValPtr = NULL;
			uint16_t		*minValPtr = NULL;
			uint16_t 		tmpValue = 0;
			uint16_t		multiplicatorBase = 0;
			uint16_t		multiplicatorPower = 0;
			uint16_t		multiplicatorPowerMax = 0;
			DLUnit_t		unit;
		} DLUInt16Data_t;

		typedef struct
		{
			uint8_t itemCount = 0;
			uint8_t selectedIndex = 0;
			uint8_t tmpSelectedIndex = 0;
			char	**valuePtr = NULL;
			uint8_t longestStringLen = 0;
		} DLCharArrayData_t;
		

		typedef struct //__attribute((__packed__)) __attribute__((__may_alias__))
		{
			// DLPageEnum_t	page = DLPAGEENUM::NOPAGE;
			uint8_t			index = 0;
			void			*valueStructPtr = NULL;
			DLItemType_t	type = DLITEMTYPE::NOTYPE;
			uint8_t			itemCount = 0;				// only needed for DLITEMTYPE::STRINGARRAY
			uint8_t			row = 0;
			uint8_t			col = 0;
			uint8_t			selectorColOffset = 0;
			uint8_t			cursorColOffset = 0;
			uint8_t			targetPageId = 0;
			DLItemAction_t	action = DLITEMACTION::NONE;
			bool			selectable = false;
			bool			editable = false;
			bool			editing = false;
			bool			visible = false;
		} DLPageItem_t;

		typedef struct //__attribute((__packed__)) __attribute__((__may_alias__))
		{
			uint8_t			index = 0;
			// char			scaffoldLines[4][21] = { 0, };
			char			*scaffoldLines[4] = { NULL, };
			DLPageItem_t	*pageItems = NULL;
			uint8_t			pageItemCount = 0;
			uint8_t			selectedItemIndex = 0;
		} DLPage_t;		
		
		DLDisplay();

		void Init(uint16_t *currentSet, uint16_t *currentSetDisplay, uint16_t *currentRead, uint16_t *voltageRead, uint16_t *powerRead, uint16_t displayUpdateIntervalMillis);
		void InitNetworkVals(uint8_t *iparray, uint8_t *nmarray, uint8_t *gwarray, char *ssid, char *psk);
		void Process();
		// void SetState(DLDisplay_t state);
		
		// DLDisplay_t GetState();
		void Refresh();
		void ButtonPressed();
		void ButtonHold();
		void EncoderUp();
		void EncoderDown();

		// void OnEncoderUp(void (*func)(uint16_t newVal));
		// void OnEncoderDown(void (*func)(uint16_t newVal));
		void OnEncoderConfirmValue(void (*func)(void *newVal, DLItemType_t type));

		uint8_t AddPage(const char *scaffoldLine1, const char *scaffoldLine2, const char *scaffoldLine3, const char *scaffoldLine4);
		uint8_t AddPageItem(uint8_t pageId, uint16_t *uint16ValPtr, uint16_t *uint16MinValPtr, uint16_t *uint16MaxValPtr, uint16_t multiplicatorBase, uint16_t multiplicatorPowerMax, DLUnit_t unit, uint8_t row, uint8_t col, uint8_t selectorColOffset, bool editable, bool selectable, bool selected = false);
		uint8_t AddPageItem(uint8_t pageId, uint8_t *IPArrayPtr, uint8_t row, uint8_t col);
		// DLPageItem_t* AddPageItem(uint8_t pageId, uint8_t *ipOct1Ptr, uint8_t *ipOct2Ptr, uint8_t *ipOct3Ptr, uint8_t *ipOct4Ptr, uint8_t row, uint8_t col, uint8_t selectorColOffset, bool editable, bool selected = false);
		// DLPageItem_t* AddPageItem(uint8_t pageId, uint32_t *uint32ValPtr, uint8_t row, uint8_t col, bool editable, bool selected = false);
		uint8_t AddPageItem(uint8_t pageId, const char *text, uint8_t row, uint8_t col, uint8_t selectorColOffset, uint8_t targetPageId, bool selectable, bool selected = false);
		uint8_t AddPageItem(uint8_t pageId, uint8_t itemCount, uint8_t row, uint8_t col, uint8_t selectorColOffset, bool selected, ...);
		uint8_t AddPageItem(uint8_t pageId, DLIcon_t icon, uint8_t row, uint8_t col, uint8_t selectorColOffset, uint8_t targetPageId, bool selectable, bool selected = false);

		void PageItemVisible(uint8_t pageId, uint8_t pageItemId, bool visible);

		
		void SetPage(uint8_t pageId);

	private:

		LiquidCrystal LCD;//(LCD_RS_PIN, LCD_EN_PIN, LCD_DB0_PIN, LCD_DB1_PIN, LCD_DB2_PIN, LCD_DB3_PIN,LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN);
		Encoder enc;//(ENC_B_PIN, ENC_A_PIN);

		// DLDisplay_t _currentState = DL_DISPLAY::NONE1;
		uint8_t _currentPage = 10;

		// DLPage_t *_currentPagePtr = NULL;

		uint16_t *_currentSet;
		uint16_t *_currentSetDisplay;
		uint16_t *_currentRead;
		uint16_t *_voltageRead;
		uint16_t *_powerRead;

		uint8_t *_IP;
		uint8_t *_NM;
		uint8_t *_GW;

		char *_SSID;
		char *_PSK;

		// void _drawValuesScaffold();
		// void _drawSettingsScaffold();
		// void _drawSettingsIPScaffold();
		// void _drawSettingsWIFIScaffold();

		void _drawPage(uint8_t pageId);

		void _drawPageValues(uint8_t pageId);

		void _setCursorPosition();

		void _displayValue(uint16_t val, bool visible, DLUnit_t unit = DLUNIT::NOUNIT);
		void _displayIP(uint8_t *ip);

		void _processDisplay();
		void _processEncoder();

		void _checkNullPtr(void *ptr);

		// DLPageItem_t ValuePage[4];
		// DLPageItem_t ConfigPage[3];
		// DLPageItem_t ConfigIpPage[4];

		DLPage_t *_pages = NULL;
		// DLPage_t *_pageItems = NULL;
		
		uint8_t _pageCount = 0;

		uint8_t _findFirstSelectableItem();
		uint8_t _findNextSelectableItem();
		uint8_t _findPrevSelectableItem();

		// DLPageItem_t *_pageItems = NULL;
		// uint8_t _pageItemCount = 0;

		uint8_t _cursorBlinkPosX = 0;
		uint8_t _cursorBlinkPosY = 0;
		bool _cursorBlink = false;

		uint16_t _iPow(uint16_t base, uint16_t power);

		uint32_t _lastDisplayUpdateMillis = 0;
		uint16_t _displayUpdateIntervalMillis = 0;

		bool _forceDisplayUpdate = false;

		uint16_t _getMultiplier();

		// Encoder stuff
		//
		long _encoderOldPosition = 0;
		long _encoderNewPosition = 0;

		void (*_EncoderUpFunction)(uint16_t newVal);
		void (*_EncoderDownFunction)(uint16_t newVal);
		void (*_EncoderConfirmValueFunction)(void *newVal, DLItemType_t type);

};