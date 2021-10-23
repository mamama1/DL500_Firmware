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
		if ((dataByte == DL_COMMAND::DATA ||
			 dataByte == DL_COMMAND::CURRENT_SET) && 
			 this->rxActive == false)
		{
			this->rxActive = true;
			this->rxStartMillis = millis();
		}

		if (this->rxActive == true)
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
		auto *const currentPacket = reinterpret_cast<DLPacket_t *const>(this->DataRXbuffer);

		uint32_t rxCRC = currentPacket->crc;

		currentPacket->crc = 0;	// remove CRC from received packet to match source packet
		uint32_t calculatedCRC = CRC32::calculate(this->DataRXbuffer, sizeof(DataRXbuffer));

		if (calculatedCRC == rxCRC)
		{
			if (currentPacket->ackRequested)
			{
				sendAck(currentPacket->packetNumber);	
			}

			memcpy(DataPacketBuffer, this->DataRXbuffer, sizeof(DataPacketBuffer));
			// this->rxComplete = true;

			if (currentPacket->Command == DL_COMMAND::DATA)
			{
				auto *const currentPacketData = reinterpret_cast<DLData_t *const>(currentPacket->dataBuffer);
				if (this->DataReceiveFunction != NULL)
				(*DataReceiveFunction)(currentPacketData);
			}
		}
		else
		{
			this->crcErrorCounter++;
		}

		this->rxBufferIndex = 0;
		this->lastRXmillis = millis();
	}
}