#include "stdafx.h"
#include "FVEDIOProtocol.h"
#include "FVEDIOException.h"
#include "Types.h"

namespace Utilities
{
	namespace FVEDIO
	{
		BYTE crc4(BYTE data[], INT32 dataSize)
		{
			UINT32 suma = 0;

			for (int i = 0; i < dataSize; i++)
			{
				suma += data[i];
			}

			return suma % 16;
		}

		void FVEDIOProtocol::Read4B(UINT16 address, BYTE data[4])
		{
			UINT8 wdata[16];
			int i, index = 0;
			
			wdata[index++] = 'R';

			//address
			char snumber[32];
			sprintf_s(snumber, sizeof(snumber), "%04X", address);
			for (i = 0; i < strlen(snumber); i++)
			{
				wdata[index++] = snumber[i];
			}
			
			//crc
			sprintf_s(snumber, sizeof(snumber), "%02X", crc4(wdata, index));
			for (i = 0; i < strlen(snumber); i++)
			{
				wdata[index++] = snumber[i];
			}

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case FVEDIOProtocolTCP:
				FVEDIOTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case FVEDIOProtocolSerial:
				FVEDIOSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			memset(data, 4, 0);
			int msgAddress = 0, msgCrc = 0, data0 = 0, data1 = 0, data2 = 0, data3 = 0;
			int par = sscanf_s((const char*)rdata, "r%04X%02X%02X%02X%02X%02X", &msgAddress, &data0, &data1, &data2, &data3, &msgCrc);
			if (par == 6)
			{
				data[0] = data0;
				data[1] = data1;
				data[2] = data2;
				data[3] = data3;
			}
			else
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Response size !=6: r{address}{data}{crc}");
				throw FVEDIOExc(GetCOMPort(), msg);
			}
		}

		void FVEDIOProtocol::Write4B(UINT16 address, BYTE data[4])
		{
			UINT8 wdata[16];
			char snumber[32];
			int i, index = 0;

			wdata[index++] = 'W';

			//address
			sprintf_s(snumber, sizeof(snumber), "%04X", address);
			for (i = 0; i < strlen(snumber); i++)
			{
				wdata[index++] = snumber[i];
			}

			//data
			sprintf_s(snumber, sizeof(snumber), "%02X%02X%02X%02X", data[0], data[1], data[2], data[3]);
			for (i = 0; i < strlen(snumber); i++)
			{
				wdata[index++] = snumber[i];
			}

			//crc
			sprintf_s(snumber, sizeof(snumber), "%02X", crc4(wdata, index));
			for (i = 0; i < strlen(snumber); i++)
			{
				wdata[index++] = snumber[i];
			}

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case FVEDIOProtocolTCP:
				FVEDIOTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case FVEDIOProtocolSerial:
				FVEDIOSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			int msgAddress = 0, msgCrc = 0;
			int par = sscanf_s((const char*)rdata, "w%04X%02X", &msgAddress, &msgCrc);
			if (par == 2)
			{
				if (msgAddress != address)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg), "Response address [%04X] != request address [%04X]", msgAddress, address);
					throw FVEDIOExc(GetCOMPort(), msg);
				}
			}
			else
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Response size !=2: r{address}{crc}");
				throw FVEDIOExc(GetCOMPort(), msg);
			}
		}

		void FVEDIOProtocol::LED(FVEDIOLED address, UINT8 intensityPercent)
		{
			BYTE data[4];
			
			if (intensityPercent > 100) intensityPercent = 100;
			double intensity = (double)intensityPercent / 100.0 * 255.0;
			data[0] = (UINT8)intensity;
			data[1] = 0x00;
			data[2] = 0x00;
			data[3] = 0x70;

			Write4B(address, data);
		}

		void FVEDIOProtocol::WriteDO(UINT16 val)
		{
			BYTE data[4];

			data[0] = 0x00;
			data[1] = 0x00;
			data[2] = (0xFF00 & val)>>8;
			data[3] = (0x00FF & val);

			Write4B(FVEDIODO, data);
		}

		void FVEDIOProtocol::WriteSingleDO(UINT8 pos, UINT8 val)
		{
			if (pos > 15)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Digital output position out of range 0-15: %d", pos);
				throw FVEDIOExc(GetCOMPort(), msg);
			}

			BYTE rdata[4];
			Read4B(FVEDIODO, rdata);

			UINT16_UT udata;
			udata.byte.byte1 = rdata[2];
			udata.byte.byte0 = rdata[3];			

			switch (pos)
			{
				case 0:
					udata.bit.b0 = (val > 0) ? 0 : 1;
					break;
				case 1:
					udata.bit.b1 = (val > 0) ? 0 : 1;
					break;
				case 2:
					udata.bit.b2 = (val > 0) ? 0 : 1;
					break;
				case 3:
					udata.bit.b3 = (val > 0) ? 0 : 1;
					break;
				case 4:
					udata.bit.b4 = (val > 0) ? 0 : 1;
					break;
				case 5:
					udata.bit.b5 = (val > 0) ? 0 : 1;
					break;
				case 6:
					udata.bit.b6 = (val > 0) ? 0 : 1;
					break;
				case 7:
					udata.bit.b7 = (val > 0) ? 0 : 1;
					break;
				case 8:
					udata.bit.b8 = (val > 0) ? 0 : 1;
					break;
				case 9:
					udata.bit.b9 = (val > 0) ? 0 : 1;
					break;
				case 10:
					udata.bit.b10 = (val > 0) ? 0 : 1;
					break;
				case 11:
					udata.bit.b11 = (val > 0) ? 0 : 1;
					break;
				case 12:
					udata.bit.b12 = (val > 0) ? 0 : 1;
					break;
				case 13:
					udata.bit.b13 = (val > 0) ? 0 : 1;
					break;
				case 14:
					udata.bit.b14 = (val > 0) ? 0 : 1;
					break;
				case 15:
					udata.bit.b15 = (val > 0) ? 0 : 1;
					break;
			}
			
			BYTE wdata[4];
			wdata[0] = rdata[0];
			wdata[1] = rdata[1];
			wdata[2] = udata.byte.byte1;
			wdata[3] = udata.byte.byte0;
			Write4B(FVEDIODO, wdata);
			
		}

		void FVEDIOProtocol::ReadDO(PUINT16 val)
		{
			BYTE data[4];
			Read4B(FVEDIODO, data);

			if (val) *val = (UINT16)data[3] + ((UINT16)data[2] << 8);
		}
	}
}