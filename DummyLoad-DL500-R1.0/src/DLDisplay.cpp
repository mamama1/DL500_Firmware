#include <DLDisplay.h>
#include <static/lcdSymbols.h>

DLDisplay::DLDisplay() :
	LCD(LCD_RS_PIN, LCD_EN_PIN, LCD_DB0_PIN, LCD_DB1_PIN, LCD_DB2_PIN, LCD_DB3_PIN,LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN),
	enc(ENC_B_PIN, ENC_A_PIN)
{
	this->LCD.begin(LCD_COLS, LCD_ROWS);
	this->_encoderOldPosition = this->enc.read();
	this->_encoderNewPosition = this->enc.read();
}

void DLDisplay::Init(uint16_t *currentSet, uint16_t *currentSetDisplay, uint16_t *currentRead, uint16_t *voltageRead, uint16_t *powerRead, uint16_t displayUpdateIntervalMillis)
{
	this->_currentSet = currentSet;
	this->_currentRead = currentRead;
	this->_voltageRead = voltageRead;
	this->_powerRead = powerRead;

	this->_currentSetDisplay = currentSetDisplay;

	this->_displayUpdateIntervalMillis = displayUpdateIntervalMillis;

	this->LCD.createChar(0, symWrench);
	// this->LCD.createChar(1, symCR1);
	// this->LCD.createChar(2, symVR1);
	// this->LCD.createChar(3, symSettings1);
	// this->LCD.createChar(4, symFull1);

	this->LCD.clear();

	// DLPageItem_t values1;
	// values1.valuePtr = this->_currentSet;
	// values1.type = DLITEMTYPE::UINT16;
	// values1.row = 1;
	// values1.col = 1;
	// values1.action = DLITEMACTION::EDIT;
	// values1.selectable = true;

	// DLPageItem_t values2;
	// values2.text = "Cfg";
	// values2.type = DLITEMTYPE::STRING;
	// values2.row = 2;
	// values2.col = 4;
	// values2.targetPage = DLPAGEENUM::CONFIG;
	// values2.action = DLITEMACTION::JUMP;
	// values2.selectable = true;

	// DLPageItem_t values3;
	// values3.valuePtr = this->_voltageRead;
	// values3.type = DLITEMTYPE::UINT16;
	// values3.row = 1;
	// values3.col = 9;
	// values3.selectable = false;

	// DLPageItem_t values4;
	// values4.valuePtr = this->_currentRead;
	// values4.type = DLITEMTYPE::UINT16;
	// values4.row = 2;
	// values4.col = 9;
	// values4.selectable = false;

	// DLPageItem_t values5;
	// values5.valuePtr = this->_powerRead;
	// values5.type = DLITEMTYPE::UINT32;
	// values5.row = 3;
	// values5.col = 9;
	// values5.selectable = false;

	// char valuesLines[4][21] = {	{"     SET|   READ|   "},
	// 							{"  0.000A| 0.000V|   "},
	// 							{"        | 0.000A|   "},
	// 							{"        | 0.000W|   "}};

	// DLPage_t pages[4];

	// pages[0].pageItems[0] = &values1;
	// pages[0].pageItems[1] = &values2;
	// pages[0].pageItems[2] = &values3;
	// pages[0].pageItems[3] = &values4;
	// pages[0].pageItems[4] = &values5;
	// pages[0].scaffoldLines = valuesLines;
	// pages[0].pageItemCount = 5;
	// pages[0].selectedItemIndex = &values1;

	// this->_pages = pages;

	// this->SetPage(DLPAGEENUM::VALUES);
}

uint8_t DLDisplay::AddPage(const char *scaffoldLine1, const char *scaffoldLine2, const char *scaffoldLine3, const char *scaffoldLine4)
{
	this->_pageCount++;

	uint8_t currentPageIndex = this->_pageCount - 1;
	
	this->_pages = (DLPage_t*)realloc(this->_pages, this->_pageCount * sizeof(DLPage_t));
	this->_checkNullPtr(this->_pages);

	this->_pages[currentPageIndex].index = currentPageIndex;

	this->_pages[currentPageIndex].scaffoldLines[0] = (char *)malloc(strnlen(scaffoldLine1, 20) + 1);
	this->_checkNullPtr(this->_pages[currentPageIndex].scaffoldLines[0]);

	this->_pages[currentPageIndex].scaffoldLines[1] = (char *)malloc(strnlen(scaffoldLine2, 20) + 1);
	this->_checkNullPtr(this->_pages[currentPageIndex].scaffoldLines[1]);

	this->_pages[currentPageIndex].scaffoldLines[2] = (char *)malloc(strnlen(scaffoldLine3, 20) + 1);
	this->_checkNullPtr(this->_pages[currentPageIndex].scaffoldLines[2]);

	this->_pages[currentPageIndex].scaffoldLines[3] = (char *)malloc(strnlen(scaffoldLine4, 20) + 1);
	this->_checkNullPtr(this->_pages[currentPageIndex].scaffoldLines[3]);
	
	strncpy(this->_pages[currentPageIndex].scaffoldLines[0], scaffoldLine1, strnlen(scaffoldLine1, 20) + 1);
	strncpy(this->_pages[currentPageIndex].scaffoldLines[1], scaffoldLine2, strnlen(scaffoldLine2, 20) + 1);
	strncpy(this->_pages[currentPageIndex].scaffoldLines[2], scaffoldLine3, strnlen(scaffoldLine3, 20) + 1);
	strncpy(this->_pages[currentPageIndex].scaffoldLines[3], scaffoldLine4, strnlen(scaffoldLine4, 20) + 1);
		
	this->_pages[currentPageIndex].pageItems = NULL;
	this->_pages[currentPageIndex].pageItemCount = 0;
	this->_pages[currentPageIndex].selectedItemIndex = 0;

	return currentPageIndex;
}

uint8_t DLDisplay::AddPageItem(uint8_t pageId, uint16_t *uint16ValPtr, uint16_t *uint16MinValPtr, uint16_t *uint16MaxValPtr, uint16_t multiplicatorBase, uint16_t multiplicatorPowerMax, DLUnit_t unit, uint8_t row, uint8_t col, uint8_t selectorColOffset, bool editable, bool selectable, bool selected)
{
	if (pageId + 1 > this->_pageCount)
		return UINT8_MAX;

	this->_pages[pageId].pageItemCount++;

	uint8_t currentPageItemIndex = this->_pages[pageId].pageItemCount - 1;
	
	this->_pages[pageId].pageItems = (DLPageItem_t*)realloc(this->_pages[pageId].pageItems, this->_pages[pageId].pageItemCount * sizeof(*this->_pages[pageId].pageItems));
	this->_checkNullPtr(this->_pages[pageId].pageItems);

	this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr = (DLUInt16Data_t*)malloc(sizeof(DLUInt16Data_t));

	auto *uint16DataStruct = reinterpret_cast<DLUInt16Data_t *>(this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr);

	uint16DataStruct->valuePtr = uint16ValPtr;
	uint16DataStruct->tmpValue = 0;
	uint16DataStruct->minValPtr = uint16MinValPtr;
	uint16DataStruct->maxValPtr = uint16MaxValPtr;
	uint16DataStruct->multiplicatorBase = multiplicatorBase;
	uint16DataStruct->multiplicatorPowerMax = multiplicatorPowerMax;
	uint16DataStruct->multiplicatorPower = 0;
	uint16DataStruct->unit = unit;
	// uint16DataStruct->visible = true;

	this->_pages[pageId].pageItems[currentPageItemIndex].index = currentPageItemIndex;
	// this->_pages[pageId].pageItems[currentPageItemIndex].valuePtr = uint16ValPtr;
	// this->_pages[pageId].pageItems[currentPageItemIndex].minValPtr = uint16MinValPtr;
	// this->_pages[pageId].pageItems[currentPageItemIndex].maxValPtr = uint16MaxValPtr;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorBase = multiplicatorBase;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorPowerMax = multiplicatorPowerMax;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorPower = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].row = row;
	this->_pages[pageId].pageItems[currentPageItemIndex].col = col;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectorColOffset = selectorColOffset;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectable = selectable;
	this->_pages[pageId].pageItems[currentPageItemIndex].editable = editable;
	this->_pages[pageId].pageItems[currentPageItemIndex].action = DLITEMACTION::EDIT;
	this->_pages[pageId].pageItems[currentPageItemIndex].type = DLITEMTYPE::UINT16;
	this->_pages[pageId].pageItems[currentPageItemIndex].cursorColOffset = 5;
	this->_pages[pageId].pageItems[currentPageItemIndex].editing = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].targetPageId = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].visible = true;
	// this->_pages[pageId].pageItems[currentPageItemIndex].tmpValue = 0;

	if (editable)
	{
		uint16DataStruct->tmpValue = *uint16ValPtr;
		// this->_pages[pageId].pageItems[currentPageItemIndex].tmpValue = *uint16ValPtr;
	}

	if (selected)
	{
		this->_pages[pageId].selectedItemIndex = this->_pages[pageId].pageItems[currentPageItemIndex].index;
	}

	return currentPageItemIndex;
}

uint8_t DLDisplay::AddPageItem(uint8_t pageId, uint8_t *IPArrayPtr, uint8_t row, uint8_t col)
{
	if (pageId + 1 > this->_pageCount)
		return UINT8_MAX;

	this->_pages[pageId].pageItemCount++;

	uint8_t currentPageItemIndex = this->_pages[pageId].pageItemCount - 1;
	
	this->_pages[pageId].pageItems = (DLPageItem_t*)realloc(this->_pages[pageId].pageItems, this->_pages[pageId].pageItemCount * sizeof(*this->_pages[pageId].pageItems));
	this->_checkNullPtr(this->_pages[pageId].pageItems);


	this->_pages[pageId].pageItems[currentPageItemIndex].index = currentPageItemIndex;
	this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr = IPArrayPtr;
	// this->_pages[pageId].pageItems[currentPageItemIndex].minValPtr = NULL;
	// this->_pages[pageId].pageItems[currentPageItemIndex].maxValPtr = NULL;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorBase = 0;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorPowerMax = 0;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorPower = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].row = row;
	this->_pages[pageId].pageItems[currentPageItemIndex].col = col;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectorColOffset = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectable = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].editable = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].action = DLITEMACTION::EDIT;
	this->_pages[pageId].pageItems[currentPageItemIndex].type = DLITEMTYPE::IP;
	this->_pages[pageId].pageItems[currentPageItemIndex].cursorColOffset = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].editing = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].targetPageId = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].visible = true;
	// this->_pages[pageId].pageItems[currentPageItemIndex].tmpValue = 0;

	return currentPageItemIndex;
}


uint8_t DLDisplay::AddPageItem(uint8_t pageId, const char *text, uint8_t row, uint8_t col, uint8_t selectorColOffset, uint8_t targetPageId, bool selectable, bool selected)
{
	if (pageId + 1 > this->_pageCount)
		return UINT8_MAX;
	
	this->_pages[pageId].pageItemCount++;

	uint8_t currentPageItemIndex = this->_pages[pageId].pageItemCount - 1;

	this->_pages[pageId].pageItems = (DLPageItem_t*)realloc(this->_pages[pageId].pageItems, this->_pages[pageId].pageItemCount * sizeof(*this->_pages[pageId].pageItems));
	this->_checkNullPtr(this->_pages[pageId].pageItems);


	this->_pages[pageId].pageItems[currentPageItemIndex].index = currentPageItemIndex;
	
	this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr = (char *)malloc(strlen(text) + 1);
	this->_checkNullPtr(this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr);
	strncpy((char *)this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr, text, strlen(text) + 1);

	// this->_pages[pageId].pageItems[currentPageItemIndex].minValPtr = NULL;
	// this->_pages[pageId].pageItems[currentPageItemIndex].maxValPtr = NULL;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorBase = 0;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorPowerMax = 0;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorPower = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].row = row;
	this->_pages[pageId].pageItems[currentPageItemIndex].col = col;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectorColOffset = selectorColOffset;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectable = selectable;
	this->_pages[pageId].pageItems[currentPageItemIndex].editable = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].targetPageId = targetPageId;
	this->_pages[pageId].pageItems[currentPageItemIndex].action = DLITEMACTION::JUMP;
	this->_pages[pageId].pageItems[currentPageItemIndex].type = DLITEMTYPE::STRING;
	this->_pages[pageId].pageItems[currentPageItemIndex].editing = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].cursorColOffset = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].visible = true;
	// this->_pages[pageId].pageItems[currentPageItemIndex].tmpValue = 0;

	if (selected)
	{
		this->_pages[pageId].selectedItemIndex = this->_pages[pageId].pageItems[currentPageItemIndex].index;
	}

	return currentPageItemIndex;
}

uint8_t DLDisplay::AddPageItem(uint8_t pageId, uint8_t itemCount, uint8_t row, uint8_t col, uint8_t selectorColOffset, bool selected, ...)
{
	if (pageId + 1 > this->_pageCount)
		return UINT8_MAX;
	
	this->_pages[pageId].pageItemCount++;

	uint8_t currentPageItemIndex = this->_pages[pageId].pageItemCount - 1;

	this->_pages[pageId].pageItems = (DLPageItem_t*)realloc(this->_pages[pageId].pageItems, this->_pages[pageId].pageItemCount * sizeof(*this->_pages[pageId].pageItems));
	this->_checkNullPtr(this->_pages[pageId].pageItems);

	this->_pages[pageId].pageItems[currentPageItemIndex].index = currentPageItemIndex;

	this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr = (DLCharArrayData_t*)malloc(sizeof(DLCharArrayData_t));
	this->_checkNullPtr(this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr);

	auto *charArrayDataStruct = reinterpret_cast<DLCharArrayData_t *>(this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr);

	charArrayDataStruct->itemCount = itemCount;
	charArrayDataStruct->selectedIndex = 0;
	charArrayDataStruct->tmpSelectedIndex = 0;
	charArrayDataStruct->longestStringLen = 0;
	charArrayDataStruct->valuePtr = (char**)malloc(sizeof(char*) * itemCount);
	this->_checkNullPtr(charArrayDataStruct->valuePtr);

	// this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr = malloc(sizeof(char*) * itemCount);

	va_list ap;
    va_start(ap, selected);

	for (uint8_t n = 0; n < itemCount; n++)
	{
		// ((char**)this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr)[n] = strdup(va_arg(ap, char*));
		charArrayDataStruct->valuePtr[n] = strdup(va_arg(ap, char*));
		this->_checkNullPtr(charArrayDataStruct->valuePtr[n]);

		uint8_t len = strlen(charArrayDataStruct->valuePtr[n]);
		if (charArrayDataStruct->longestStringLen < len)
		{
			charArrayDataStruct->longestStringLen = len;
		}
	}

	this->_pages[pageId].pageItems[currentPageItemIndex].row = row;
	this->_pages[pageId].pageItems[currentPageItemIndex].col = col;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectorColOffset = selectorColOffset;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectable = true;
	this->_pages[pageId].pageItems[currentPageItemIndex].editable = true;
	this->_pages[pageId].pageItems[currentPageItemIndex].targetPageId = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].action = DLITEMACTION::EDIT;
	this->_pages[pageId].pageItems[currentPageItemIndex].type = DLITEMTYPE::STRINGARRAY;
	this->_pages[pageId].pageItems[currentPageItemIndex].editing = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].cursorColOffset = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].visible = true;
	// this->_pages[pageId].pageItems[currentPageItemIndex].tmpValue = 0;

	if (selected)
	{
		this->_pages[pageId].selectedItemIndex = this->_pages[pageId].pageItems[currentPageItemIndex].index;
	}

	return currentPageItemIndex;
}

uint8_t DLDisplay::AddPageItem(uint8_t pageId, DLIcon_t icon, uint8_t row, uint8_t col, uint8_t selectorColOffset, uint8_t targetPageId, bool selectable, bool selected)
{
	if (pageId + 1 > this->_pageCount)
		return UINT8_MAX;
	
	this->_pages[pageId].pageItemCount++;

	uint8_t currentPageItemIndex = this->_pages[pageId].pageItemCount - 1;

	this->_pages[pageId].pageItems = (DLPageItem_t*)realloc(this->_pages[pageId].pageItems, this->_pages[pageId].pageItemCount * sizeof(*this->_pages[pageId].pageItems));
	this->_checkNullPtr(this->_pages[pageId].pageItems);


	this->_pages[pageId].pageItems[currentPageItemIndex].index = currentPageItemIndex;
	
	this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr = (DLIcon_t*)malloc(sizeof(DLIcon_t));
	this->_checkNullPtr(this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr);
	*(DLIcon_t*)this->_pages[pageId].pageItems[currentPageItemIndex].valueStructPtr = icon;

	// this->_pages[pageId].pageItems[currentPageItemIndex].minValPtr = NULL;
	// this->_pages[pageId].pageItems[currentPageItemIndex].maxValPtr = NULL;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorBase = 0;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorPowerMax = 0;
	// this->_pages[pageId].pageItems[currentPageItemIndex].multiplicatorPower = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].row = row;
	this->_pages[pageId].pageItems[currentPageItemIndex].col = col;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectorColOffset = selectorColOffset;
	this->_pages[pageId].pageItems[currentPageItemIndex].selectable = selectable;
	this->_pages[pageId].pageItems[currentPageItemIndex].editable = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].targetPageId = targetPageId;
	this->_pages[pageId].pageItems[currentPageItemIndex].action = DLITEMACTION::JUMP;
	this->_pages[pageId].pageItems[currentPageItemIndex].type = DLITEMTYPE::ICON;
	this->_pages[pageId].pageItems[currentPageItemIndex].editing = false;
	this->_pages[pageId].pageItems[currentPageItemIndex].cursorColOffset = 0;
	this->_pages[pageId].pageItems[currentPageItemIndex].visible = true;
	// this->_pages[pageId].pageItems[currentPageItemIndex].tmpValue = 0;

	if (selected)
	{
		this->_pages[pageId].selectedItemIndex = this->_pages[pageId].pageItems[currentPageItemIndex].index;
	}

	return currentPageItemIndex;
}

void DLDisplay::PageItemVisible(uint8_t pageId, uint8_t pageItemId, bool visible)
{
	if (pageId > this->_pageCount)
		return;

	if (pageItemId > this->_pages[pageId].pageItemCount)
		return;

	this->_pages[pageId].pageItems[pageItemId].selectable = visible;
	this->_pages[pageId].pageItems[pageItemId].visible = visible;

	// if (this->_pages[pageId].pageItems[pageItemId].type == DLITEMTYPE::UINT16)
	// {
	// 	auto *uint16DataStruct = reinterpret_cast<DLUInt16Data_t *>(this->_pages[pageId].pageItems[pageItemId].valueStructPtr);

	// 	// uint16DataStruct->visible = visible;
	// 	uint16DataStruct->visible = visible;
	// 	// LOG("visible: %u\r\n", visible);
	// }
}

void DLDisplay::SetPage(uint8_t pageId)
{
	if (pageId >= this->_pageCount)
		return;

	if (this->_currentPage != pageId)
	{
		this->_currentPage = pageId;
		this->_pages[this->_currentPage].selectedItemIndex = this->_findFirstSelectableItem();
		_drawPage(pageId);
	}

	this->_currentPage = pageId;	
	this->Refresh();
}

uint8_t DLDisplay::_findFirstSelectableItem()
{
	if (this->_pages[this->_currentPage].pageItemCount == 1)
	{
		return this->_pages[this->_currentPage].selectedItemIndex;
	}
		
	if (this->_pages[this->_currentPage].pageItemCount == 0)
	{
		return 0;
	}

	for (uint8_t n = 0; n < this->_pages[this->_currentPage].pageItemCount; n++)
	{
		if (this->_pages[this->_currentPage].pageItems[n].selectable)
		{
			return this->_pages[this->_currentPage].pageItems[n].index;
		}
	}

	return 0;
}

uint8_t DLDisplay::_findNextSelectableItem()
{
	if (this->_pages[this->_currentPage].pageItemCount == 1)
	{
		return this->_pages[this->_currentPage].selectedItemIndex;
	}
		
	if (this->_pages[this->_currentPage].pageItemCount == 0)
	{
		return 0;
	}
		

	// uint8_t startIndex = this->_pages[this->_currentPage].selectedItemIndex + 1;

	// if (this->_pages[this->_currentPage].selectedItemIndex == this->_pages[this->_currentPage].pageItemCount - 1)
	// {
	// 	// We are on the last pageItem, start from the first item
	// 	startIndex = 0;
	// }

	for (uint8_t n = this->_pages[this->_currentPage].selectedItemIndex + 1; n < this->_pages[this->_currentPage].pageItemCount; n++)
	{
		if (n > this->_pages[this->_currentPage].pageItemCount - 1)
			continue;

		if (this->_pages[this->_currentPage].pageItems[n].selectable)
		{
			return this->_pages[this->_currentPage].pageItems[n].index;
		}
	}

	for (uint8_t n = 0; n < this->_pages[this->_currentPage].pageItemCount; n++)
	{
		if (this->_pages[this->_currentPage].pageItems[n].selectable)
		{
			return this->_pages[this->_currentPage].pageItems[n].index;
		}
	}
	
	return 0;
}

uint8_t DLDisplay::_findPrevSelectableItem()
{
	if (this->_pages[this->_currentPage].pageItemCount == 1)
	{
		return this->_pages[this->_currentPage].selectedItemIndex;
	}
	
	if (this->_pages[this->_currentPage].pageItemCount == 0)
	{
		return 0;
	}
	
	for (uint8_t n = this->_pages[this->_currentPage].selectedItemIndex - 1; n >= 0; n--)
	{
		if (n > this->_pages[this->_currentPage].pageItemCount - 1)
			continue;

		if (this->_pages[this->_currentPage].pageItems[n].selectable)
		{
			return this->_pages[this->_currentPage].pageItems[n].index;
		}
	}

	for (uint8_t n = this->_pages[this->_currentPage].pageItemCount - 1; n >= 0; n--)
	{
		if (this->_pages[this->_currentPage].pageItems[n].selectable)
		{
			return this->_pages[this->_currentPage].pageItems[n].index;
		}
	}

	return 0;
}

void DLDisplay::_checkNullPtr(void *ptr)
{
	if (ptr == NULL)
	{
		LOG("MALLOC FAILED\r\n");
		while(true);
	}	
}

void DLDisplay::InitNetworkVals(uint8_t *iparray, uint8_t *nmarray, uint8_t *gwarray, char *ssid, char *psk)
{
	this->_IP = iparray;
	this->_NM = nmarray;
	this->_GW = gwarray;

	this->_SSID = ssid;
	this->_PSK = psk;
}

void DLDisplay::Process()
{
	this->_processDisplay();
	this->_processEncoder();	
}

void DLDisplay::_processDisplay()
{
	if ((millis() - this->_lastDisplayUpdateMillis > this->_displayUpdateIntervalMillis) || this->_forceDisplayUpdate)
	{
		this->_drawPageValues(this->_currentPage);

		this->_setCursorPosition();

		this->_lastDisplayUpdateMillis = millis();
		this->_forceDisplayUpdate = false;
	}
}

void DLDisplay::_setCursorPosition()
{
	if (this->_cursorBlink)
	{
		this->LCD.setCursor(this->_cursorBlinkPosX, this->_cursorBlinkPosY);
	}
}

void DLDisplay::_processEncoder()
{
	this->_encoderNewPosition = this->enc.read() / 4;
	if (this->_encoderNewPosition != this->_encoderOldPosition)
	{
		// LOG("old: %u\r\nnew: %u\r\n\r\n", this->_encoderOldPosition, this->_encoderNewPosition);

		if (this->_encoderNewPosition > this->_encoderOldPosition)
		{
			if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editing)
			{
				// LOG("current page: %u, selectedItemIndex: %u, editing: %u\r\n", this->_currentPage, this->_pages[this->_currentPage].selectedItemIndex, this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editing);
				if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editable)
				{
					if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].type == DLITEMTYPE::UINT16)
					{
						auto *uint16DataStruct = reinterpret_cast<DLUInt16Data_t *>(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].valueStructPtr);

						if (uint16DataStruct->tmpValue + this->_iPow(uint16DataStruct->multiplicatorBase, uint16DataStruct->multiplicatorPower) <
							*uint16DataStruct->maxValPtr)
						{
							// LOG("val: %u, pow: %u\r\n", uint16DataStruct->tmpValue, this->_iPow(uint16DataStruct->multiplicatorBase, uint16DataStruct->multiplicatorPower));
							// LOG("base: %u, power: %u\r\n", uint16DataStruct->multiplicatorBase, uint16DataStruct->multiplicatorPower);
							uint16DataStruct->tmpValue += this->_iPow(uint16DataStruct->multiplicatorBase, uint16DataStruct->multiplicatorPower);
							this->Refresh();
						}
						else
						{
							uint16DataStruct->tmpValue = *uint16DataStruct->maxValPtr;
							this->Refresh();
						}
					}
					else if(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].type == DLITEMTYPE::STRINGARRAY)
					{
						auto *charArrayDataStruct = reinterpret_cast<DLCharArrayData_t *>(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].valueStructPtr);

						if (charArrayDataStruct->tmpSelectedIndex + 1 < charArrayDataStruct->itemCount)
						{
							charArrayDataStruct->tmpSelectedIndex++;
							LOG("idx: %u\r\n", charArrayDataStruct->tmpSelectedIndex);
							this->Refresh();
						}
						else
						{
							charArrayDataStruct->tmpSelectedIndex = 0;
							LOG("idx: %u\r\n", charArrayDataStruct->tmpSelectedIndex);
							this->Refresh();
						}
					}
				}				
			}
			else
			{
				this->_pages[this->_currentPage].selectedItemIndex = this->_findNextSelectableItem();
				LOG("selected index: %u\r\n", this->_pages[this->_currentPage].selectedItemIndex);
				this->Refresh();
			}
			// else if (this->_pages[this->_currentPage].pageItems[1])

			// if (this->_EncoderUpFunction != NULL)
			// 	(*_EncoderUpFunction)(*this->_currentSetDisplay + this->_getMultiplier());
			
			// onEncoderUp
			// if (this->_currentState == DL_DISPLAY::VALUES)
			// 	this->SetState(DL_DISPLAY::CONFIG);
			// else if (this->_currentState == DL_DISPLAY::CONFIG)
			// 	this->SetState(DL_DISPLAY::VALUES);
			// else if (this->_currentState >= DL_DISPLAY::EDIT_E && this->_currentState <= DL_DISPLAY::EDIT_T)
			// {
			// 	if (this->_EncoderUpFunction != NULL)
			// 		(*_EncoderUpFunction)(*this->_currentSetDisplay + this->_getMultiplier());
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_WIFI);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_WIFI)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_EXIT);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_EXIT)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP_NM);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_NM)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP_GW);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_GW)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP_EXIT);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_EXIT)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP_IP);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_1)
			// {
			// 	this->_IP[0]++;
			// 	this->Refresh();
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_2)
			// {
			// 	this->_IP[1]++;
			// 	this->Refresh();
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_3)
			// {
			// 	this->_IP[2]++;
			// 	this->Refresh();
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_4)
			// {
			// 	this->_IP[3]++;
			// 	this->Refresh();
			// }
			// this->_encoderOldPosition = this->_encoderNewPosition;			
		}
		else if (this->_encoderNewPosition < this->_encoderOldPosition)
		{
			if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editing)
			{
				if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editable)
				{
					if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].type == DLITEMTYPE::UINT16)
					{
						auto *uint16DataStruct = reinterpret_cast<DLUInt16Data_t *>(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].valueStructPtr);

						if (uint16DataStruct->tmpValue > this->_iPow(uint16DataStruct->multiplicatorBase, uint16DataStruct->multiplicatorPower) && 
							uint16DataStruct->tmpValue - this->_iPow(uint16DataStruct->multiplicatorBase, uint16DataStruct->multiplicatorPower) >
							*uint16DataStruct->minValPtr)
						{
							uint16DataStruct->tmpValue -= this->_iPow(uint16DataStruct->multiplicatorBase, uint16DataStruct->multiplicatorPower);
							this->Refresh();
						}
						else
						{
							uint16DataStruct->tmpValue = *uint16DataStruct->minValPtr;
							this->Refresh();
						}
					}
					else if(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].type == DLITEMTYPE::STRINGARRAY)
					{
						auto *charArrayDataStruct = reinterpret_cast<DLCharArrayData_t *>(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].valueStructPtr);

						if (charArrayDataStruct->tmpSelectedIndex > 0)
						{
							charArrayDataStruct->tmpSelectedIndex--;
							LOG("idx: %u\r\n", charArrayDataStruct->tmpSelectedIndex);
							this->Refresh();
						}
						else
						{
							charArrayDataStruct->tmpSelectedIndex = charArrayDataStruct->itemCount - 1;
							LOG("idx: %u\r\n", charArrayDataStruct->tmpSelectedIndex);
							this->Refresh();
						}
					}
				}
			}
			else
			{
				this->_pages[this->_currentPage].selectedItemIndex = this->_findPrevSelectableItem();
				this->Refresh();
			}

			// if (this->_EncoderDownFunction != NULL)
			// 	(*_EncoderDownFunction)(*this->_currentSetDisplay - this->_getMultiplier());

			// onEncoderDown
			// if (this->_currentState == DL_DISPLAY::VALUES)
			// 	this->SetState(DL_DISPLAY::CONFIG);
			// else if (this->_currentState == DL_DISPLAY::CONFIG)
			// 	this->SetState(DL_DISPLAY::VALUES);
			// else if (this->_currentState >= DL_DISPLAY::EDIT_E && this->_currentState <= DL_DISPLAY::EDIT_T)
			// {
			// 	if (this->_EncoderDownFunction != NULL)
			// 		(*_EncoderDownFunction)(*this->_currentSetDisplay - this->_getMultiplier());
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_EXIT);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_EXIT)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_WIFI);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_WIFI)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP_EXIT);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_NM)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP_IP);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_GW)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP_NM);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_EXIT)
			// {
			// 	this->SetState(DL_DISPLAY::CONFIG_IP_GW);
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_1)
			// {
			// 	this->_IP[0]--;
			// 	this->Refresh();
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_2)
			// {
			// 	this->_IP[1]--;
			// 	this->Refresh();
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_3)
			// {
			// 	this->_IP[2]--;
			// 	this->Refresh();
			// }
			// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_4)
			// {
			// 	this->_IP[3]--;
			// 	this->Refresh();
			// }
			// this->_encoderOldPosition = this->_encoderNewPosition;
		}
		this->_encoderOldPosition = this->_encoderNewPosition;
	}
}

// void DLDisplay::OnEncoderUp(void (*func)(uint16_t newVal))
// {
// 	this->_EncoderUpFunction = func;
// }

// void DLDisplay::OnEncoderDown(void (*func)(uint16_t newVal))
// {
// 	this->_EncoderDownFunction = func;
// }

void DLDisplay::OnEncoderConfirmValue(void (*func)(void *newVal, DLItemType_t type))
{
	this->_EncoderConfirmValueFunction = func;
}

void DLDisplay::EncoderUp()
{

}

void DLDisplay::EncoderDown()
{

}

uint16_t DLDisplay::_iPow(uint16_t base, uint16_t power)
{
	uint16_t retVal = base;
	for (uint8_t n = 1; n < power; n++)
	{
		retVal *= base;
	}

	if (power == 0)
		return 1;
	else
		return retVal;
}

void DLDisplay::ButtonPressed()
{
	if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].action == DLITEMACTION::EDIT &&
		this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editing == false)
	{
		// Serial.print("enable editing");

		this->_cursorBlink = true;
		this->LCD.cursor();
		this->LCD.blink();
		this->_cursorBlinkPosX = this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].col + 
									this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].cursorColOffset;
		this->_cursorBlinkPosY = this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].row;

		this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editing = true;
		this->_setCursorPosition();
		this->Refresh();
	}
	else if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].action == DLITEMACTION::EDIT &&
			 this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editing == true)
	{
		// this->_increaseValuePower();
		if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].type == DLITEMTYPE::UINT16)
		{
			auto *uint16DataStruct = reinterpret_cast<DLUInt16Data_t *>(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].valueStructPtr);
			
			uint16DataStruct->multiplicatorPower++;
			if (uint16DataStruct->multiplicatorPower > uint16DataStruct->multiplicatorPowerMax)
			{
				uint16DataStruct->multiplicatorPower = 0;
			}
			this->_cursorBlinkPosX = this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].col + 
									this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].cursorColOffset -
									uint16DataStruct->multiplicatorPower;

			if (uint16DataStruct->multiplicatorPower >= 3)
			{
				this->_cursorBlinkPosX -= 1;
			}
			// LOG("multiPower: %u\r\n", uint16DataStruct->multiplicatorPower);
			this->_setCursorPosition();
		}
	}
	else if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].action == DLITEMACTION::JUMP)
	{
		this->SetPage(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].targetPageId);
	}



	// else
	// {
	// 	this->_pages[this->_currentPage].selectedItemIndex->editing = false;
	// }
	// if (this->_currentState == DL_DISPLAY::VALUES)
	// {
	// 	this->SetState(DL_DISPLAY::EDIT_T);
	// }
	// else if (this->_currentState == DL_DISPLAY::EDIT_T)
	// {
	// 	this->SetState(DL_DISPLAY::EDIT_H);
	// }
	// else if (this->_currentState == DL_DISPLAY::EDIT_H)
	// {
	// 	this->SetState(DL_DISPLAY::EDIT_Z);
	// }
	// else if (this->_currentState == DL_DISPLAY::EDIT_Z)
	// {
	// 	this->SetState(DL_DISPLAY::EDIT_E);
	// }
	// else if (this->_currentState == DL_DISPLAY::EDIT_E)
	// {
	// 	this->SetState(DL_DISPLAY::EDIT_T);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG)
	// {
	// 	this->SetState(DL_DISPLAY::CONFIG_IP);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG_EXIT)
	// {
	// 	this->SetState(DL_DISPLAY::VALUES);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG_IP)
	// {
	// 	this->SetState(DL_DISPLAY::CONFIG_IP_IP);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_EXIT)
	// {
	// 	this->SetState(DL_DISPLAY::CONFIG_IP);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP)
	// {
	// 	this->SetState(DL_DISPLAY::CONFIG_IP_IP_EDIT_1);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_1)
	// {
	// 	this->SetState(DL_DISPLAY::CONFIG_IP_IP_EDIT_2);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_2)
	// {
	// 	this->SetState(DL_DISPLAY::CONFIG_IP_IP_EDIT_3);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_3)
	// {
	// 	this->SetState(DL_DISPLAY::CONFIG_IP_IP_EDIT_4);
	// }
	// else if (this->_currentState == DL_DISPLAY::CONFIG_IP_IP_EDIT_4)
	// {
	// 	this->SetState(DL_DISPLAY::CONFIG_IP_IP);
	// }
}

void DLDisplay::ButtonHold()
{
	if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].action == DLITEMACTION::EDIT &&
		this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editing == true)
	{
		this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].editing = false;
		this->LCD.noBlink();
		this->LCD.noCursor();

		if (this->_EncoderConfirmValueFunction != NULL)
		{
			if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].type == DLITEMTYPE::UINT16)
			{
				auto *uint16DataStruct = reinterpret_cast<DLUInt16Data_t *>(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].valueStructPtr);
				uint16DataStruct->multiplicatorPower = 0;

				*uint16DataStruct->valuePtr = uint16DataStruct->tmpValue;
				(*_EncoderConfirmValueFunction)(uint16DataStruct->valuePtr, DLITEMTYPE::UINT16);
			}
			else if (this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].type == DLITEMTYPE::STRINGARRAY)
			{
				auto *charArrayDataStruct = reinterpret_cast<DLCharArrayData_t *>(this->_pages[this->_currentPage].pageItems[this->_pages[this->_currentPage].selectedItemIndex].valueStructPtr);
				charArrayDataStruct->selectedIndex = charArrayDataStruct->tmpSelectedIndex;
				(*_EncoderConfirmValueFunction)(charArrayDataStruct->valuePtr[charArrayDataStruct->selectedIndex], DLITEMTYPE::STRINGARRAY);
			}
		}
	}		
}

// DLDisplay::DLDisplay_t DLDisplay::GetState()
// {
// 	return this->_currentState;
// }

void DLDisplay::_drawPage(uint8_t pageId)
{
	// if (page == DLPAGEENUM::VALUES)
	// {
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print("     SET|   READ|   "); this->LCD.setCursor(0, 1);
	// 	this->LCD.print("> 0.000A| 0.000V|   "); this->LCD.setCursor(0, 2);
	// 	this->LCD.print("    Cfg | 0.000A|   "); this->LCD.setCursor(0, 3);
	// 	this->LCD.print("        | 0.000W|   ");

	// 	this->LCD.setCursor(2, 2);
	// 	this->LCD.write(byte(3));
	// }
	// else if (page == DLPAGEENUM::CONFIG)
	// {
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print("> IP Configuration  "); this->LCD.setCursor(0, 1);
	// 	this->LCD.print("  WiFi Configuration"); this->LCD.setCursor(0, 2);
	// 	this->LCD.print("  Exit              "); this->LCD.setCursor(0, 3);
	// 	this->LCD.print("                    ");
	// }
	// else if (page == DLPAGEENUM::CONFIG_IP)
	// {
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print("> IP:               "); this->LCD.setCursor(0, 1);
	// 	this->LCD.print("  SM:               "); this->LCD.setCursor(0, 2);
	// 	this->LCD.print("  GW:               "); this->LCD.setCursor(0, 3);
	// 	this->LCD.print("  Exit              ");
	// }
	// else if (page == DLPAGEENUM::CONFIG_WIFI)
	// {
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print("  SSID:             "); this->LCD.setCursor(0, 1);
	// 	this->LCD.print("  PSK:              "); this->LCD.setCursor(0, 2);
	// 	this->LCD.print("> Exit              "); this->LCD.setCursor(0, 3);
	// 	this->LCD.print("                    ");
	// }

	// for (uint8_t n = 0; n <= this->_pages[page].pageItemCount; n++)
	// {
	// 	this->LCD.setCursor(this->_pages[page].pageItems[n].col, this->_pages[page].pageItems[n].row);
	// 	if (this->_pages[page].pageItems[n].type == DLITEMTYPE::INTEGER)
	// 		this->_displayValue((*(uint32_t*)(this->_pages[page].pageItems[n].valuePtr)));
	// 	else if (this->_pages[page].pageItems[n].type == DLITEMTYPE::STRING)
	// 		this->LCD.print((*(char*)(this->_pages[page].pageItems[n].text)));
	// }

	// Serial.println("print scaffold");

	this->LCD.setCursor(0, 0);
	for (uint8_t n = 0; n < 4; n++)
	{
		this->LCD.setCursor(0, n);
		this->LCD.print(this->_pages[pageId].scaffoldLines[n]);
		// Serial.print(this->_pages[pageId].scaffoldLines[n]);
		uint8_t trailingSpaces = 20 - strnlen(this->_pages[pageId].scaffoldLines[n], 20);
		for (uint8_t x = 0; x < trailingSpaces; x++)
		{
			// Serial.print("-");
			this->LCD.print(" ");
		}
		// Serial.println();
	}
}

void DLDisplay::_drawPageValues(uint8_t pageId)
{
	uint8_t selectedItemIndex = this->_pages[this->_currentPage].selectedItemIndex;
	LOG("---\r\n");
	for (uint8_t n = 0; n < this->_pages[pageId].pageItemCount; n++)
	{
		if (this->_pages[pageId].pageItems[n].visible == false)
		{
			continue;
		}
		
		if (this->_pages[this->_currentPage].pageItems[selectedItemIndex].index == this->_pages[pageId].pageItems[n].index)
		{
			this->LCD.setCursor(this->_pages[pageId].pageItems[n].col - this->_pages[pageId].pageItems[n].selectorColOffset, this->_pages[pageId].pageItems[n].row);
			this->LCD.print(">");
			LOG("print '>'\r\n");
		}
		else if (this->_pages[this->_currentPage].pageItems[selectedItemIndex].index != this->_pages[pageId].pageItems[n].index)
		{
			this->LCD.setCursor(this->_pages[pageId].pageItems[n].col - this->_pages[pageId].pageItems[n].selectorColOffset, this->_pages[pageId].pageItems[n].row);
			this->LCD.print(" ");
			LOG("print ' '\r\n");
		}
		// Serial.println(this->_pages[pageId].pageItems[n].col);
		// Serial.println(this->_pages[pageId].pageItems[n].row);

		this->LCD.setCursor(this->_pages[pageId].pageItems[n].col, this->_pages[pageId].pageItems[n].row);

		if (this->_pages[pageId].pageItems[n].type == DLITEMTYPE::UINT16)
		{
			auto *uint16DataStruct = reinterpret_cast<DLUInt16Data_t *>(this->_pages[pageId].pageItems[n].valueStructPtr);

			if (this->_pages[pageId].pageItems[n].selectable)
			{
				// Serial.println(this->_pages[pageId].pageItems[n].tmpValue);
				// LOG("%u: %u\r\n", uint16DataStruct->tmpValue, this->_pages[pageId].pageItems[n].visible);
				this->_displayValue(uint16DataStruct->tmpValue, true || this->_pages[pageId].pageItems[n].editing, uint16DataStruct->unit);
			}
			else
			{
				// Serial.println(*(uint16_t*)this->_pages[pageId].pageItems[n].valuePtr);
				// LOG("%u: %u\r\n", *uint16DataStruct->valuePtr, this->_pages[pageId].pageItems[n].visible);
				this->_displayValue(*uint16DataStruct->valuePtr, true || this->_pages[pageId].pageItems[n].editing, uint16DataStruct->unit);
				// uint16_t value = *((uint16_t *)this->_pages[pageId].pageItems[n].valuePtr);
				// this->_displayValue(value);

			}
			// this->_displayValue((*(uint32_t*)(this->_pages[page].pageItems[n].valuePtr)));
		}
		else if (this->_pages[pageId].pageItems[n].type == DLITEMTYPE::STRING)
		{
			// Serial.println(this->_pages[pageId].pageItems[n].text);
			this->LCD.print((const char *)this->_pages[pageId].pageItems[n].valueStructPtr);

			// this->LCD.print((*this->_pages[page].pageItems[n].text));
			// this->LCD.print((*(char*)(this->_pages[page].pageItems[n].text)));
		}
		else if (this->_pages[pageId].pageItems[n].type == DLITEMTYPE::ICON)
		{
			this->LCD.write(byte(*(DLIcon_t *)this->_pages[pageId].pageItems[n].valueStructPtr));
		}
		// else if (this->_pages[pageId].pageItems[n].type == DLITEMTYPE::UINT32)
		// {
		// 	if (this->_pages[pageId].pageItems[n].selectable)
		// 	{
		// 		this->_displayValue(this->_pages[pageId].pageItems[n].tmpValue);
		// 	}
		// 	else
		// 	{
		// 		this->_displayValue(*(uint32_t*)this->_pages[pageId].pageItems[n].valuePtr);
		// 	}
		// 	// this->_displayValue((*(uint32_t*)(this->_pages[page].pageItems[n].valuePtr)));
		// }
		else if (this->_pages[pageId].pageItems[n].type == DLITEMTYPE::IP)
		{
			this->_displayIP((uint8_t*)this->_pages[pageId].pageItems[n].valueStructPtr);
		}
		else if (this->_pages[pageId].pageItems[n].type == DLITEMTYPE::STRINGARRAY)
		{
			auto *charArrayDataStruct = reinterpret_cast<DLCharArrayData_t *>(this->_pages[pageId].pageItems[n].valueStructPtr);
			this->LCD.print(charArrayDataStruct->valuePtr[charArrayDataStruct->tmpSelectedIndex]);

			// overwrite last string if it was longer than the current one
			uint8_t len = strlen(charArrayDataStruct->valuePtr[charArrayDataStruct->tmpSelectedIndex]);
			uint8_t diff = charArrayDataStruct->longestStringLen - len;

			if (diff > 0)
			{
				for(uint8_t n = 0; n < diff; n++)
				{
					this->LCD.print(" ");
				}
			}
		}
	}
	LOG("---\r\n");

	// if (page == DLPAGEENUM::VALUES)
	// {
	// 	this->LCD.setCursor(1, 1);
	// 	this->_displayValue(*(this->_currentSetDisplay));

	// 	this->LCD.setCursor(9, 1);
	// 	this->_displayValue(*(this->_voltageRead));

	// 	this->LCD.setCursor(9, 2);
	// 	this->_displayValue(*(this->_currentRead));

	// 	this->LCD.setCursor(9, 3);
	// 	this->_displayValue(*(this->_powerRead));
	// }
	// else if (page == DLPAGEENUM::CONFIG_IP)
	// {
	// 	this->LCD.setCursor(5, 0);
	// 	this->_displayIP(this->_IP);

	// 	this->LCD.setCursor(5, 1);
	// 	this->_displayIP(this->_NM);

	// 	this->LCD.setCursor(5, 2);
	// 	this->_displayIP(this->_GW);
	// }
}

// uint16_t DLDisplay::_getMultiplier()
// {
// 	if (this->_currentState == DLDisplay::DL_DISPLAY::EDIT_E)
// 	{
// 		return 1;
// 	}
// 	else if (this->_currentState == DLDisplay::DL_DISPLAY::EDIT_Z)
// 	{
// 		return 10;
// 	}
// 	else if (this->_currentState == DLDisplay::DL_DISPLAY::EDIT_H)
// 	{
// 		return 100;
// 	}
// 	else if (this->_currentState == DLDisplay::DL_DISPLAY::EDIT_T)
// 	{
// 		return 1000;
// 	}
	
// 	return 1;
// }

void DLDisplay::Refresh()
{
	this->_forceDisplayUpdate = true;
}

void DLDisplay::_displayValue(uint16_t val, bool visible, DLUnit_t unit)
{
	if (visible == false)
	{
		// LOG("%u: hidden", val);
		//this->LCD.print("--.---");
		return;
	}

	if (val < 10000)
	{
		this->LCD.print(" ");
	}

	this->LCD.print((uint16_t)(val / 1000));
	this->LCD.print(".");

	val %= 1000;

	if (val < 10)
	{
		this->LCD.print("00");
	}
	else if (val < 100)
	{
		this->LCD.print("0");
	}
	
	this->LCD.print(val);
	//LOG("%c\r\n", 'a');
	// LOG("%u\r\n", val);

	if (unit == DLUNIT::AMPERE)
	{
		this->LCD.print("A");
	}
	else if (unit == DLUNIT::VOLT)
	{
		this->LCD.print("V");
	}
	else if (unit == DLUNIT::WATT)
	{
		this->LCD.print("W");
	}
	else if (unit == DLUNIT::OHM)
	{
		this->LCD.print("O");
	}
}

void DLDisplay::_displayIP(uint8_t *ip)
{
	for (uint8_t n = 0; n <= 3; n++)
	{
		if (ip[n] < 100)
			this->LCD.print("0");
		if (ip[n] < 10)
			this->LCD.print("0");
		
		this->LCD.print(ip[n]);

		if (n < 3)
			this->LCD.print(".");
	}
}


// void DLDisplay::SetState(DLDisplay_t state)
// {
	// if ((this->_currentState >= DL_DISPLAY::CONFIG + 1 && state < DL_DISPLAY::CONFIG + 1) || this->_currentState == DL_DISPLAY::NONE)
	// {
	// 	this->_drawValuesScaffold();
	// }
	// else if (((this->_currentState < DL_DISPLAY::CONFIG + 1 || this->_currentState > DL_DISPLAY::CONFIG_EXIT) && (state >= DL_DISPLAY::CONFIG + 1 && state <= DL_DISPLAY::CONFIG_EXIT)) || this->_currentState == DL_DISPLAY::NONE)
	// {
	// 	this->_drawSettingsScaffold();
	// }
	// else if (this->_currentState < DL_DISPLAY::CONFIG_IP_IP && state >= DL_DISPLAY::CONFIG_IP_IP)
	// {
	// 	this->_drawSettingsIPScaffold();
	// }

	// if (state == DL_DISPLAY::VALUES)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(">");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(" ");
	// 	this->_cursorBlink = false;
	// }
	// else if (state == DL_DISPLAY::EDIT_E)
	// {
	// 	this->LCD.setCursor(6, 1);
	// 	this->_cursorBlinkPosX = 6;
	// 	this->_cursorBlinkPosY = 1;
	// 	this->_cursorBlink = true;
	// 	this->LCD.cursor();
	// 	this->LCD.blink();
	// }
	// else if (state == DL_DISPLAY::EDIT_Z)
	// {
	// 	this->LCD.setCursor(5, 1);
	// 	this->_cursorBlinkPosX = 5;
	// 	this->_cursorBlinkPosY = 1;
	// 	this->_cursorBlink = true;
	// 	this->LCD.cursor();
	// 	this->LCD.blink();
	// }
	// else if (state == DL_DISPLAY::EDIT_H)
	// {
	// 	this->LCD.setCursor(4, 1);
	// 	this->_cursorBlinkPosX = 4;
	// 	this->_cursorBlinkPosY = 1;
	// 	this->_cursorBlink = true;
	// 	this->LCD.cursor();
	// 	this->LCD.blink();
	// }
	// else if (state == DL_DISPLAY::EDIT_T)
	// {
	// 	this->LCD.setCursor(2, 1);
	// 	this->_cursorBlinkPosX = 2;
	// 	this->_cursorBlinkPosY = 1;
	// 	this->_cursorBlink = true;
	// 	this->LCD.cursor();
	// 	this->LCD.blink();
	// }
	// else if (state == DL_DISPLAY::CONFIG)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(">");
	// 	this->_cursorBlink = false;
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print(">");
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(" ");
	// 	this->_cursorBlink = false;
	// }
	// else if (state == DL_DISPLAY::CONFIG_WIFI)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(">");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(" ");
	// 	this->_cursorBlink = false;
	// }
	// else if (state == DL_DISPLAY::CONFIG_EXIT)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(">");
	// 	this->_cursorBlink = false;
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP_IP)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print(">");
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 3);
	// 	this->LCD.print(" ");
	// 	this->_cursorBlink = false;
	// 	this->Refresh();
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP_NM)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(">");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 3);
	// 	this->LCD.print(" ");
	// 	this->_cursorBlink = false;
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP_GW)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(">");
	// 	this->LCD.setCursor(0, 3);
	// 	this->LCD.print(" ");
	// 	this->_cursorBlink = false;
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP_EXIT)
	// {
	// 	this->LCD.noCursor();
	// 	this->LCD.noBlink();
	// 	this->LCD.setCursor(0, 0);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 1);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 2);
	// 	this->LCD.print(" ");
	// 	this->LCD.setCursor(0, 3);
	// 	this->LCD.print(">");
	// 	this->_cursorBlink = false;
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP_IP_EDIT_1)
	// {
	// 	this->LCD.setCursor(7, 0);
	// 	this->_cursorBlinkPosX = 7;
	// 	this->_cursorBlinkPosY = 0;
	// 	this->_cursorBlink = true;
	// 	this->LCD.cursor();
	// 	this->LCD.blink();
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP_IP_EDIT_2)
	// {
	// 	this->LCD.setCursor(11, 0);
	// 	this->_cursorBlinkPosX = 11;
	// 	this->_cursorBlinkPosY = 0;
	// 	this->_cursorBlink = true;
	// 	this->LCD.cursor();
	// 	this->LCD.blink();
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP_IP_EDIT_3)
	// {
	// 	this->LCD.setCursor(15, 0);
	// 	this->_cursorBlinkPosX = 15;
	// 	this->_cursorBlinkPosY = 0;
	// 	this->_cursorBlink = true;
	// 	this->LCD.cursor();
	// 	this->LCD.blink();
	// }
	// else if (state == DL_DISPLAY::CONFIG_IP_IP_EDIT_4)
	// {
	// 	this->LCD.setCursor(19, 0);
	// 	this->_cursorBlinkPosX = 19;
	// 	this->_cursorBlinkPosY = 0;
	// 	this->_cursorBlink = true;
	// 	this->LCD.cursor();
	// 	this->LCD.blink();
	// }


// 	this->_currentState = state;	
// }

// void DLDisplay::_drawValuesScaffold()
// {
// 	this->LCD.setCursor(0, 0);
// 	this->LCD.print("     SET|   READ|   "); this->LCD.setCursor(0, 1);
// 	this->LCD.print("> 0.000A| 0.000V|   "); this->LCD.setCursor(0, 2);
// 	this->LCD.print("    Cfg | 0.000A|   "); this->LCD.setCursor(0, 3);
// 	this->LCD.print("        | 0.000W|   ");

// 	this->LCD.setCursor(2, 2);
// 	this->LCD.write(byte(3));
// }

// void DLDisplay::_drawSettingsScaffold()
// {
// 	this->LCD.setCursor(0, 0);
// 	this->LCD.print("> IP Configuration  "); this->LCD.setCursor(0, 1);
// 	this->LCD.print("  WiFi Configuration"); this->LCD.setCursor(0, 2);
// 	this->LCD.print("  Exit              "); this->LCD.setCursor(0, 3);
// 	this->LCD.print("                    ");
// }

// void DLDisplay::_drawSettingsIPScaffold()
// {
// 	this->LCD.setCursor(0, 0);
// 	this->LCD.print("> IP:               "); this->LCD.setCursor(0, 1);
// 	this->LCD.print("  SM:               "); this->LCD.setCursor(0, 2);
// 	this->LCD.print("  GW:               "); this->LCD.setCursor(0, 3);
// 	this->LCD.print("  Exit              ");
// }

// void DLDisplay::_drawSettingsWIFIScaffold()
// {
// 	this->LCD.setCursor(0, 0);
// 	this->LCD.print("  SSID:             "); this->LCD.setCursor(0, 1);
// 	this->LCD.print("  PSK:              "); this->LCD.setCursor(0, 2);
// 	this->LCD.print("> Exit              "); this->LCD.setCursor(0, 3);
// 	this->LCD.print("                    ");
// }