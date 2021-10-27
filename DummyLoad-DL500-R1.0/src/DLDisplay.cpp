#include <DLDisplay.h>
#include <static/lcdSymbols.h>

DLDisplay::DLDisplay() :
	LCD(LCD_RS_PIN, LCD_EN_PIN, LCD_DB0_PIN, LCD_DB1_PIN, LCD_DB2_PIN, LCD_DB3_PIN,LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN),
	enc(ENC_B_PIN, ENC_A_PIN)
{
	this->LCD.begin(LCD_COLS, LCD_ROWS);
	this->_encoderOldPosition = enc.read();
}

void DLDisplay::Init(DLDisplay_t initialState, uint16_t *currentSet, uint16_t *currentSetDisplay, uint16_t *currentRead, uint16_t *voltageRead, uint16_t *powerRead, uint16_t displayUpdateIntervalMillis)
{
	this->_currentSet = currentSet;
	this->_currentRead = currentRead;
	this->_voltageRead = voltageRead;
	this->_powerRead = powerRead;

	this->_currentSetDisplay = currentSetDisplay;

	this->_displayUpdateIntervalMillis = displayUpdateIntervalMillis;

	this->LCD.createChar(0, symCL1);
	this->LCD.createChar(1, symCR1);
	this->LCD.createChar(2, symVR1);
	this->LCD.createChar(3, symSettings1);
	this->LCD.createChar(4, symFull1);

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
	// pages[0].selectedItem = &values1;

	// this->_pages = pages;

	// this->SetPage(DLPAGEENUM::VALUES);
}

uint8_t DLDisplay::AddPage(const char *scaffoldLine1, const char *scaffoldLine2, const char *scaffoldLine3, const char *scaffoldLine4)
{
	this->_pageCount++;
	this->_pages = (DLPage_t*)realloc(this->_pages, this->_pageCount * sizeof(DLPage_t));
	
	this->_checkNullPtr(this->_pages);
	
	this->_pages[this->_pageCount - 1].scaffoldLines[0] = &scaffoldLine1;
	this->_pages[this->_pageCount - 1].scaffoldLines[1] = &scaffoldLine2;
	this->_pages[this->_pageCount - 1].scaffoldLines[2] = &scaffoldLine3;
	this->_pages[this->_pageCount - 1].scaffoldLines[3] = &scaffoldLine4;
	
	this->_pages[this->_pageCount - 1].pageItems = NULL;
	this->_pages[this->_pageCount - 1].pageItemCount = 0;
	this->_pages[this->_pageCount - 1].selectedItem = NULL;

	// *(this->_pages[this->_pageCount - 1]).scaffoldLines[0] = *scaffoldLine1;
	// *(this->_pages[this->_pageCount - 1]).scaffoldLines[2] = *scaffoldLine2;
	// *(this->_pages[this->_pageCount - 1]).scaffoldLines[3] = *scaffoldLine3;
	// *(this->_pages[this->_pageCount - 1]).scaffoldLines[4] = *scaffoldLine4;

	// return &this->_pages[this->_pageCount - 1];
	return this->_pageCount - 1;
}

DLDisplay::DLPageItem_t* DLDisplay::AddPageItem(uint8_t pageId, uint16_t *uint16ValPtr, uint8_t row, uint8_t col, bool editable, bool selected)
{
	if (pageId + 1 > this->_pageCount)
		return NULL;

	this->_pages[pageId].pageItemCount++;
	this->_pages[pageId].pageItems = (DLPageItem_t*)realloc(this->_pages[pageId].pageItems, this->_pages[pageId].pageItemCount * sizeof(DLPageItem_t));

	this->_checkNullPtr(this->_pages[pageId].pageItems);

	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].valuePtr = uint16ValPtr;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].row = row;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].col = col;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].selectable = editable;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].action = DLITEMACTION::EDIT;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].type = DLITEMTYPE::UINT16;

	if (editable)
	{
		this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].tmpValue = *uint16ValPtr;
	}

	if (selected)
		this->_pages[pageId].selectedItem = &this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1];

	return &this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1];
}

// DLDisplay::DLPageItem_t* DLDisplay::AddPageItem(uint8_t pageId, uint32_t *uint32ValPtr, uint8_t row, uint8_t col, bool editable, bool selected)
// {
// 	if (pageId + 1 > this->_pageCount)
// 		return NULL;

// 	this->_pages[pageId].pageItemCount++;
// 	this->_pages[pageId].pageItems = (DLPageItem_t*)realloc(this->_pages[pageId].pageItems, this->_pages[pageId].pageItemCount * sizeof(DLPageItem_t));
	
// 	this->_checkNullPtr(this->_pages[pageId].pageItems);

// 	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].valuePtr = uint32ValPtr;
// 	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].row = row;
// 	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].col = col;
// 	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].selectable = editable;
// 	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].action = DLITEMACTION::EDIT;
// 	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].type = DLITEMTYPE::UINT32;

// 	if (editable)
// 	{
// 		this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].tmpValue = *uint32ValPtr;
// 	}

// 	if (selected)
// 		this->_pages[pageId].selectedItem = &this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1];

// 	return &this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1];
// }

DLDisplay::DLPageItem_t* DLDisplay::AddPageItem(uint8_t pageId, const char *text, uint8_t row, uint8_t col, uint8_t targetPageId, bool selectable, bool selected)
{
	if (pageId + 1 > this->_pageCount)
		return NULL;
	
	this->_pages[pageId].pageItemCount++;
	this->_pages[pageId].pageItems = (DLPageItem_t*)realloc(this->_pages[pageId].pageItems, this->_pages[pageId].pageItemCount * sizeof(DLPageItem_t));
	
	// // page->pageItems = (DLPageItem_t*)realloc(page->pageItems, page->pageItemCount * sizeof(DLPageItem_t));

	// // this->_pageItemCount++;
	// // this->_pageItems = (DLPageItem_t*)realloc(this->_pageItems, this->_pageItemCount * sizeof(DLPageItem_t));
	
	this->_checkNullPtr(this->_pages[pageId].pageItems);
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].text = text;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].row = row;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].col = col;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].selectable = selectable;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].targetPageId = targetPageId;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].action = DLITEMACTION::JUMP;
	this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1].type = DLITEMTYPE::STRING;
	

	if (selected)
		this->_pages[pageId].selectedItem = &this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1];

	return &this->_pages[pageId].pageItems[this->_pages[pageId].pageItemCount - 1];
}

void DLDisplay::SetPage(uint8_t pageId)
{
	if (pageId >= this->_pageCount)
		return;

	if (this->_currentPage != pageId)
	{
		this->_currentPage = pageId;
		_drawPage(pageId);
	}

	this->_currentPage = pageId;	
}

void DLDisplay::_checkNullPtr(void *ptr)
{
	if (ptr == NULL)
	{
		Serial.println("MALLOC FAILED");
		while(true){};
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
		// Serial.println("Draw values");
		this->_drawPageValues(this->_currentPage);
		// Serial.println("Draw values end");
		if (this->_cursorBlink)
		{
			this->LCD.setCursor(this->_cursorBlinkPosX, this->_cursorBlinkPosY);
		}

		this->_lastDisplayUpdateMillis = millis();
		this->_forceDisplayUpdate = false;
	}
}

void DLDisplay::_processEncoder()
{
	long encoderNewPosition = this->enc.read();
	if (encoderNewPosition != this->_encoderOldPosition)
	{
		if (encoderNewPosition > this->_encoderOldPosition + 4)
		{
			if (this->_pages[this->_currentPage].selectedItem->editing)
			{
				this->_pages[this->_currentPage].selectedItem->tmpValue++;
				this->Refresh();
			}

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
			this->_encoderOldPosition = encoderNewPosition;			
		}
		else if (encoderNewPosition < this->_encoderOldPosition - 4)
		{
			if (this->_pages[this->_currentPage].selectedItem->editing)
			{
				this->_pages[this->_currentPage].selectedItem->tmpValue--;
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
			this->_encoderOldPosition = encoderNewPosition;
		}
	}
}

void DLDisplay::OnEncoderUp(void (*func)(uint16_t newVal))
{
	this->_EncoderUpFunction = func;
}

void DLDisplay::OnEncoderDown(void (*func)(uint16_t newVal))
{
	this->_EncoderDownFunction = func;
}

void DLDisplay::OnEncoderConfirmValue(void (*func)(uint16_t newVal))
{
	this->_EncoderConfirmValueFunction = func;
}

void DLDisplay::EncoderUp()
{

}

void DLDisplay::EncoderDown()
{

}

void DLDisplay::ButtonPressed()
{
	if (this->_pages[this->_currentPage].selectedItem->action == DLITEMACTION::EDIT &&
		this->_pages[this->_currentPage].selectedItem->editing == false)
	{
		this->_pages[this->_currentPage].selectedItem->editing = true;
	}



	// else
	// {
	// 	this->_pages[this->_currentPage].selectedItem->editing = false;
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
	if (this->_pages[this->_currentPage].selectedItem->action == DLITEMACTION::EDIT &&
		this->_pages[this->_currentPage].selectedItem->editing == true)
	{
		this->_pages[this->_currentPage].selectedItem->editing = false;

		if (this->_EncoderConfirmValueFunction != NULL)
		{
			*(uint16_t*)this->_pages[this->_currentPage].selectedItem->valuePtr = this->_pages[this->_currentPage].selectedItem->tmpValue;
			(*_EncoderConfirmValueFunction)(*(uint16_t*)this->_pages[this->_currentPage].selectedItem->valuePtr);
		}
	}		
}

DLDisplay::DLDisplay_t DLDisplay::GetState()
{
	return this->_currentState;
}

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
	for (uint8_t n = 0; n <= 3; n++)
	{
		this->LCD.setCursor(0, n);
		this->LCD.print(*this->_pages[pageId].scaffoldLines[n]);
	}
}

void DLDisplay::_drawPageValues(uint8_t pageId)
{
	for (uint8_t n = 0; n < this->_pages[pageId].pageItemCount; n++)
	{
		if (this->_pages[pageId].selectedItem == &this->_pages[pageId].pageItems[n])
		{
			this->LCD.setCursor(0, this->_pages[pageId].pageItems[n].row);
			this->LCD.print(">");
		}
		// Serial.println(this->_pages[pageId].pageItems[n].col);
		// Serial.println(this->_pages[pageId].pageItems[n].row);

		this->LCD.setCursor(this->_pages[pageId].pageItems[n].col, this->_pages[pageId].pageItems[n].row);

		if (this->_pages[pageId].pageItems[n].type == DLITEMTYPE::UINT16)
		{
			if (this->_pages[pageId].pageItems[n].selectable)
			{
				// Serial.println(this->_pages[pageId].pageItems[n].tmpValue);
				this->_displayValue(this->_pages[pageId].pageItems[n].tmpValue);
			}
			else
			{
				// Serial.println(*(uint16_t*)this->_pages[pageId].pageItems[n].valuePtr);
				this->_displayValue(*(uint16_t*)this->_pages[pageId].pageItems[n].valuePtr);
				// uint16_t value = *((uint16_t *)this->_pages[pageId].pageItems[n].valuePtr);
				// this->_displayValue(value);

			}
			// this->_displayValue((*(uint32_t*)(this->_pages[page].pageItems[n].valuePtr)));
		}
		else if (this->_pages[pageId].pageItems[n].type == DLITEMTYPE::STRING)
		{
			// Serial.println(this->_pages[pageId].pageItems[n].text);
			this->LCD.print(this->_pages[pageId].pageItems[n].text);

			// this->LCD.print((*this->_pages[page].pageItems[n].text));
			// this->LCD.print((*(char*)(this->_pages[page].pageItems[n].text)));
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
		// else if (this->_pages[page].pageItems[n].type == DLITEMTYPE::IP)
		// 	this->_displayIP((*(char*)(this->_pages[page].pageItems[n].valuePtr)));
	}

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

uint16_t DLDisplay::_getMultiplier()
{
	if (this->_currentState == DLDisplay::DL_DISPLAY::EDIT_E)
	{
		return 1;
	}
	else if (this->_currentState == DLDisplay::DL_DISPLAY::EDIT_Z)
	{
		return 10;
	}
	else if (this->_currentState == DLDisplay::DL_DISPLAY::EDIT_H)
	{
		return 100;
	}
	else if (this->_currentState == DLDisplay::DL_DISPLAY::EDIT_T)
	{
		return 1000;
	}
	
	return 1;
}

void DLDisplay::Refresh()
{
	this->_forceDisplayUpdate = true;
}

void DLDisplay::_displayValue(uint16_t val)
{
	if (val < 10000)
	{
		this->LCD.print(' ');
	}

	this->LCD.print((uint16_t)(val / 1000));
	this->LCD.print('.');

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


void DLDisplay::SetState(DLDisplay_t state)
{
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


	this->_currentState = state;	
}

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