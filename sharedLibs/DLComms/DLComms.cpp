#include <Arduino.h>
#include <DLComms.h>

void DLComms::OnDataReceive(void (*func)(DLData_t *const data))
{
	this->DataReceiveFunction = func;
}

void DLComms::Process()
{
	while (this->Seriala.available())
	{
		uint8_t dataByte = this->Seriala.read();
		if (dataByte == static_cast<uint8_t>(DLComms::DL_COMMAND::DATA) && this->rxActive == false)
		{
			this->DataRXbuffer[0] = dataByte;
			this->rxActive = true;
			this->rxBufferIndex = 1;
			this->rxStartMillis = millis();
		}
		else if (this->rxActive == true)
		{
			if (millis() - RX_TIMEOUT_MILLIS > this->rxStartMillis)
			{
				this->rxActive = false;
				this->rxBufferIndex = 0;
				break;
			}

			this->DataRXbuffer[this->rxBufferIndex] = dataByte;
			this->rxBufferIndex += 1;
			if (this->rxBufferIndex >= sizeof(DataRXbuffer))
			{
				this->rxActive = false;
				break;
			}
		}

		yield();
	}

	if (this->rxBufferIndex == sizeof(DataRXbuffer))
	{
		auto *const valuePacket = reinterpret_cast<DLComms::DLData_t *const>(this->DataRXbuffer);
		uint32_t rxCRC = valuePacket->CRC;
		valuePacket->CRC = 0;	// remove CRC from received packet to match source packet
		uint32_t calculatedCRC = CRC32::calculate(this->DataRXbuffer, sizeof(DataRXbuffer));

		if (calculatedCRC == rxCRC)
		{
			memcpy(DataPacket, this->DataRXbuffer, sizeof(DataPacket));
			this->rxComplete = true;

			if (this->DataReceiveFunction != NULL)
				(*DataReceiveFunction)(valuePacket);
			// Serial.print("milliAmpsSetVal: ");
			// Serial.println(valuePacket->currentSet);

			// Serial.print("voltsReadVal: ");
			// Serial.println(valuePacket->voltageRead);

			// Serial.print("milliAmpsReadVal: ");
			// Serial.println(valuePacket->currentRead);

			// Serial.print("lastRX: ");
			// Serial.println(lastRXmillis);

			// Serial.print("millis: ");
			// Serial.println(millis());

			// Serial.print("diff: ");
			// Serial.println(millis() - lastRXmillis);

			// Serial.println();
		}
		else
		{
			this->crcErrorCounter++;
			// Serial.println("#### CRC error ####");
		}

		this->rxBufferIndex = 0;
		this->lastRXmillis = millis();
	}
}