#include <Arduino.h>
#include "SoftwareSerial.h"
#include <DLComms.h>

SoftwareSerial swSer1;
DLComms comm(swSer1);

void setup() {
  swSer1.begin(28800, SWSERIAL_8N1, D1, D2, false, 256);
  Serial.begin(115200);
  
  comm.OnDataReceive([](DLComms::DLData_t *data) {
			Serial.println(data->currentRead);
			Serial.println(data->currentSet);
			Serial.println(data->voltageRead);
			Serial.println(data->powerRead);
			});
}

void loop()
{
	comm.Process();
}


// void checkSWSerial(SoftwareSerial *ss);
// void checkSWSerial(SoftwareSerial *ss)
// {
	// while (ss->available())
	// {
	// 	uint8_t dataByte = ss->read();
	// 	if (dataByte == static_cast<uint8_t>(DLComms::DL_COMMAND::DATA) && rxActive == false)
	// 	{
	// 		DataRXbuffer[0] = dataByte;
	// 		rxActive = true;
	// 		rxBufferIndex = 1;
	// 		rxStartMillis = millis();
	// 	}
	// 	else if (rxActive == true)
	// 	{
	// 		if (millis() - RX_TIMEOUT_MILLIS > rxStartMillis)
	// 		{
	// 			rxActive = false;
	// 			rxBufferIndex = 0;
	// 			break;
	// 		}

	// 		DataRXbuffer[rxBufferIndex] = dataByte;
	// 		rxBufferIndex += 1;
	// 		if (rxBufferIndex >= sizeof(DataRXbuffer))
	// 		{
	// 			rxActive = false;
	// 			break;
	// 		}
	// 	}

	// 	yield();
	// }

	// if (rxBufferIndex == sizeof(DataRXbuffer))
	// {
	// 	auto *const valuePacket = reinterpret_cast<DLComms::DLData_t *const>(DataRXbuffer);
	// 	uint32_t rxCRC = valuePacket->CRC;
	// 	valuePacket->CRC = 0;	// remove CRC from received packet to match source packet
	// 	uint32_t calculatedCRC = CRC32::calculate(DataRXbuffer, sizeof(DataRXbuffer));

	// 	if (calculatedCRC == rxCRC)
	// 	{
	// 		memcpy(DataPacket, DataRXbuffer, sizeof(DataPacket));
	// 		rxComplete = true;
	// 		// Serial.print("milliAmpsSetVal: ");
	// 		// Serial.println(valuePacket->currentSet);

	// 		// Serial.print("voltsReadVal: ");
	// 		// Serial.println(valuePacket->voltageRead);

	// 		// Serial.print("milliAmpsReadVal: ");
	// 		// Serial.println(valuePacket->currentRead);

	// 		// Serial.print("lastRX: ");
	// 		// Serial.println(lastRXmillis);

	// 		// Serial.print("millis: ");
	// 		// Serial.println(millis());

	// 		// Serial.print("diff: ");
	// 		// Serial.println(millis() - lastRXmillis);

	// 		// Serial.println();
	// 	}
	// 	else
	// 	{
	// 		crcErrorCounter++;
	// 		// Serial.println("#### CRC error ####");
	// 	}

	// 	rxBufferIndex = 0;
	// 	lastRXmillis = millis();
	// }
// }