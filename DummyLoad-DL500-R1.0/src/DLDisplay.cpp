#include <DLDisplay.h>
#include <static/lcdSymbols.h>

DLDisplay::DLDisplay() :
	LCD(LCD_RS_PIN, LCD_EN_PIN, LCD_DB0_PIN, LCD_DB1_PIN, LCD_DB2_PIN, LCD_DB3_PIN,LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN),
	enc(ENC_B_PIN, ENC_A_PIN)
{
	this->LCD.begin(LCD_COLS, LCD_ROWS);
	this->_encoderOldPosition = enc.read();
}

void DLDisplay::Init(DLDisplay_t initialState, uint16_t *currentSet, uint16_t *currentRead, uint16_t *voltageRead, uint32_t *powerRead, uint16_t displayUpdateIntervalMillis)
{
	this->_currentSet = currentSet;
	this->_currentRead = currentRead;
	this->_voltageRead = voltageRead;
	this->_powerRead = powerRead;

	this->_displayUpdateIntervalMillis = displayUpdateIntervalMillis;

	this->LCD.createChar(0, symCL1);
	this->LCD.createChar(1, symCR1);
	this->LCD.createChar(2, symVR1);
	this->LCD.createChar(3, symSettings1);
	this->LCD.createChar(4, symFull1);

	this->SetState(initialState);
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
		if (this->_currentState < DL_DISPLAY::CONFIG + 1)
		{
			this->LCD.setCursor(1, 1);
			this->_displayValue(*(this->_currentSet));

			this->LCD.setCursor(9, 1);
			this->_displayValue(*(this->_voltageRead));

			this->LCD.setCursor(9, 2);
			this->_displayValue(*(this->_currentRead));

			this->LCD.setCursor(9, 3);
			this->_displayValue(*(this->_powerRead));
			
			if (this->_cursorBlink)
			{
				this->LCD.setCursor(this->_cursorBlinkPosX, this->_cursorBlinkPosY);
			}
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
			// onEncoderUp
			if (this->_EncoderUpFunction != NULL)
				(*_EncoderUpFunction)();

			this->_encoderOldPosition = encoderNewPosition;
		}
		else if (encoderNewPosition < this->_encoderOldPosition - 4)
		{
			// onEncoderDown
			if (this->_EncoderDownFunction != NULL)
				(*_EncoderDownFunction)();

			this->_encoderOldPosition = encoderNewPosition;
		}
	}
}

void DLDisplay::OnEncoderUp(void (*func)())
{
	this->_EncoderUpFunction = func;
}

void DLDisplay::OnEncoderDown(void (*func)())
{
	this->_EncoderDownFunction = func;
}

void DLDisplay::EncoderUp()
{

}

void DLDisplay::EncoderDown()
{

}

void DLDisplay::ButtonPressed()
{
	if (this->_currentState == DL_DISPLAY::VALUES)
	{
		this->SetState(DL_DISPLAY::EDIT_T);
	}
	else if (this->_currentState == DL_DISPLAY::EDIT_T)
	{
		this->SetState(DL_DISPLAY::EDIT_H);
	}
	else if (this->_currentState == DL_DISPLAY::EDIT_H)
	{
		this->SetState(DL_DISPLAY::EDIT_Z);
	}
	else if (this->_currentState == DL_DISPLAY::EDIT_Z)
	{
		this->SetState(DL_DISPLAY::EDIT_E);
	}
	else if (this->_currentState == DL_DISPLAY::EDIT_E)
	{
		this->SetState(DL_DISPLAY::VALUES);
	}
}

void DLDisplay::SetState(DLDisplay_t state)
{
	if ((this->_currentState >= DL_DISPLAY::CONFIG + 1 && state < DL_DISPLAY::CONFIG + 1) || this->_currentState == DL_DISPLAY::NONE)
	{
		this->_drawValuesScaffold();
	}
	else if ((this->_currentState < DL_DISPLAY::CONFIG + 1 && state >= DL_DISPLAY::CONFIG + 1) || this->_currentState == DL_DISPLAY::NONE)
	{
		this->_drawSettingsScaffold();
	}

	if (state == DL_DISPLAY::VALUES)
	{
		this->LCD.noCursor();
		this->LCD.noBlink();
		this->LCD.setCursor(0, 1);
		this->LCD.print(">");
		this->LCD.setCursor(0, 2);
		this->LCD.print(" ");
		this->_cursorBlink = false;
	}
	else if (state == DL_DISPLAY::EDIT_E)
	{
		this->LCD.setCursor(6, 1);
		this->_cursorBlinkPosX = 6;
		this->_cursorBlinkPosY = 1;
		this->_cursorBlink = true;
		this->LCD.cursor();
		this->LCD.blink();
	}
	else if (state == DL_DISPLAY::EDIT_Z)
	{
		this->LCD.setCursor(5, 1);
		this->_cursorBlinkPosX = 5;
		this->_cursorBlinkPosY = 1;
		this->_cursorBlink = true;
		this->LCD.cursor();
		this->LCD.blink();
	}
	else if (state == DL_DISPLAY::EDIT_H)
	{
		this->LCD.setCursor(4, 1);
		this->_cursorBlinkPosX = 4;
		this->_cursorBlinkPosY = 1;
		this->_cursorBlink = true;
		this->LCD.cursor();
		this->LCD.blink();
	}
	else if (state == DL_DISPLAY::EDIT_T)
	{
		this->LCD.setCursor(2, 1);
		this->_cursorBlinkPosX = 2;
		this->_cursorBlinkPosY = 1;
		this->_cursorBlink = true;
		this->LCD.cursor();
		this->LCD.blink();
	}
	else if (state == DL_DISPLAY::CONFIG)
	{
		this->LCD.noCursor();
		this->LCD.noBlink();
		this->LCD.setCursor(0, 1);
		this->LCD.print(" ");
		this->LCD.setCursor(0, 2);
		this->LCD.print(">");
		this->_cursorBlink = false;
	}

	this->_currentState = state;	
}

void DLDisplay::_drawValuesScaffold()
{
	this->LCD.setCursor(0, 0);
	this->LCD.print("     SET|   READ|   "); this->LCD.setCursor(0, 1);
	this->LCD.print("> 0.000A| 0.000V|   "); this->LCD.setCursor(0, 2);
	this->LCD.print("  Config| 0.000A|   "); this->LCD.setCursor(0, 3);
	this->LCD.print("        | 0.000W|   ");

	this->LCD.setCursor(2, 0);
	this->LCD.write(byte(3));
}

void DLDisplay::_drawSettingsScaffold()
{

}

void DLDisplay::Refresh()
{
	this->_forceDisplayUpdate = true;
}

void DLDisplay::_displayValue(uint16_t val)
{
	if (val < 10000)
		this->LCD.print(' ');

	this->LCD.print((int)(val / 1000));
	this->LCD.print('.');

	val %= 1000;

	if (val < 10)
		this->LCD.print("00");
	else if (val < 100)
		this->LCD.print("0");
	
	this->LCD.print(val);

	// LOG("XXX = %u\r\n", val);	
}