#include <Arduino.h>
#include <DLComms.h>

void DLComms::OnDataReceive(void (*func)(DLData_t *const data))
{
	this->DataReceiveFunction = func;
}

void DLComms::sendAck(uint8_t packetNumber)
{
	auto *const ackPacket = reinterpret_cast<DLPacket_t *const>(this->DataTXbuffer);

	ackPacket->Command = DL_COMMAND::ACK;
	ackPacket->packetNumber = packetNumber;
	ackPacket->crc = 0;

	uint32_t ackCRC = CRC32::calculate(this->DataTXbuffer, sizeof(ackPacket));

	ackPacket->crc = ackCRC;

	this->Seriala.write(this->DataTXbuffer, sizeof(this->DataTXbuffer));
}

void DLComms::sendData(uint16_t currentSet, uint16_t currentRead, uint16_t voltageRead, uint32_t powerRead, bool requestAck)
{
	auto *const sendPacket = reinterpret_cast<DLPacket_t *const>(this->DataTXbuffer);

	sendPacket->ackRequested = requestAck;
	sendPacket->Command = DL_COMMAND::DATA;
	sendPacket->packetNumber = 0;
	sendPacket->crc = 0;
	
	auto *const sendData = reinterpret_cast<DLData_t *const>(sendPacket->dataBuffer);

	sendData->currentSet = currentSet;
	sendData->currentRead = currentRead;
	sendData->voltageRead = voltageRead;
	sendData->powerRead = powerRead;

	uint32_t checksum = CRC32::calculate(this->DataTXbuffer, sizeof(this->DataTXbuffer));

	sendPacket->crc = checksum;

	// for (uint8_t n = 0; n < sizeof(this->DataTXbuffer); n++)
	// {
	// 	Serial.print(this->DataTXbuffer[n], HEX);
	// 	Serial.print(" ");
	// }
	// Serial.println();

	this->Seriala.write(this->DataTXbuffer, sizeof(this->DataTXbuffer));
}

void DLComms::Process()
{
	while (this->Seriala.available())
	{
		uint8_t dataByte = this->Seriala.read();
		// Serial.print(dataByte, HEX);
		// Serial.print(" ");
		if ((dataByte == static_cast<uint8_t>(DL_COMMAND::DATA) ||
			 dataByte == static_cast<uint8_t>(DL_COMMAND::CURRENT_SET)) && 
			 this->rxActive == false)
		{
			// this->DataRXbuffer[0] = dataByte;
			this->rxActive = true;
			// this->rxBufferIndex = 1;
			this->rxStartMillis = millis();
			// Serial.print("1");
		}

		if (this->rxActive == true)
		{
			if (millis() - RX_TIMEOUT_MILLIS > this->rxStartMillis)
			{
				this->rxActive = false;
				this->rxBufferIndex = 0;
				// Serial.print("2");
				break;
			}

			// Serial.print("3");
			this->DataRXbuffer[this->rxBufferIndex] = dataByte;
			this->rxBufferIndex += 1;
			if (this->rxBufferIndex >= sizeof(DataRXbuffer))
			{
				this->rxActive = false;
				// Serial.print("4");
				break;
			}
		}

		yield();
	}

	if (this->rxBufferIndex == sizeof(DataRXbuffer))
	{
		// Serial.print("5");
		auto *const currentPacket = reinterpret_cast<DLPacket_t *const>(this->DataRXbuffer);

		uint32_t rxCRC = currentPacket->crc;
		// Serial.println(rxCRC);

		currentPacket->crc = 0;	// remove CRC from received packet to match source packet
		uint32_t calculatedCRC = CRC32::calculate(this->DataRXbuffer, sizeof(DataRXbuffer));

		// for (uint8_t n = 0; n < sizeof(DataRXbuffer); n++)
		// {
		// 	Serial.print(DataRXbuffer[n], HEX);
		// 	Serial.print(" ");
		// }
		// Serial.println();

		// Serial.println(calculatedCRC);

		if (calculatedCRC == rxCRC)
		{
			// Serial.print("6");
			if (currentPacket->ackRequested)
			{
				// Serial.print("7");
				sendAck(currentPacket->packetNumber);	
			}

			memcpy(DataPacketBuffer, this->DataRXbuffer, sizeof(DataPacketBuffer));
			// this->rxComplete = true;

			if (currentPacket->Command == DL_COMMAND::DATA)
			{
				// Serial.print("8");
				auto *const currentPacketData = reinterpret_cast<DLData_t *const>(currentPacket->dataBuffer);
				if (this->DataReceiveFunction != NULL)
				(*DataReceiveFunction)(currentPacketData);
			}

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
			// Serial.print("9");
			this->crcErrorCounter++;
			// Serial.println("#### CRC error ####");
		}

		this->rxBufferIndex = 0;
		this->lastRXmillis = millis();
	}
}