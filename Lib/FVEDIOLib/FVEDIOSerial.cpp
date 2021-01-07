#include "stdafx.h"
#include "FVEDIOSerial.h"
#include "FVEDIOException.h"

#ifdef _DEBUG
#define FVEDIOSERIAL_LOG
#endif

namespace Utilities
{
	namespace FVEDIO
	{
#ifdef FVEDIOSERIAL_LOG
		void Log(const char* COMPort, const char* id, UINT8 data[], UINT32 dataSize, SYSTEMTIME t)
		{
			vector<char> sbuffer(dataSize*3 + 64);
			char* pend = sbuffer.data();

			pend += sprintf(pend, "[%02d:%02d:%02d.%03d] %s[%s] 0x%x ", (int)t.wHour, (int)t.wMinute, (int)t.wSecond, (int)t.wMilliseconds, id, COMPort, GetCurrentThreadId());
			
			//data
			pend += sprintf(pend, "[");
			for (unsigned int i = 1; i < dataSize - 1; i++)
			{
				*pend = data[i];
				pend++;
			}
			//pend--;
			pend += sprintf(pend, "]\n");
			printf(sbuffer.data());
		}
#endif

		void FVEDIOSerial::Query(UINT8 WriteData[], UINT16 WriteCount, UINT8 ReadData[], PUINT32 ReadCount)
		{
			CAutoLock al(m_FVEDIOSerialLock);

			unsigned int ReadDataAlloc = *ReadCount;
			*ReadCount = 0;

			//Header
			vector<UINT8> wdata(WriteCount + 2);
			unsigned int wdataSize = 0;

			wdata[0] = 0x02;

			int i;
			for (i = 0; i < WriteCount; i++)
			{
				wdata[i + 1] = WriteData[i];
			}
			i += 1;
			wdata[i++] = 0x03;
			wdataSize = i;

			if (wdataSize > 17)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Message too big >17bytes, actual size: %d bytes", wdataSize);
				throw FVEDIOExc(GetCOMPort(), msg);
			}

			vector<UINT8> rdata(ReadDataAlloc);
			unsigned int rdataSize = ReadDataAlloc;

#ifdef FVEDIOSERIAL_LOG
			SYSTEMTIME wt;
			GetSystemTime(&wt);
			Log(GetCOMPort(), "W", wdata.data(), wdataSize, wt);
#endif
			try
			{
				Serial::SetTermChar(true, 0x03);
				Serial::Query(wdata.data(), wdataSize, rdata.data(), &rdataSize);
			}
			catch (char* &e)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "%s", (const char*)e);
				throw FVEDIOExc(GetCOMPort(), msg);
			}

#ifdef FVEDIOSERIAL_LOG
			SYSTEMTIME rt;
			GetSystemTime(&rt);
			Log(GetCOMPort(), "R", rdata.data(), rdataSize, rt);
#endif

			if ((rdataSize == 17 && rdata[0] == 0x02 || rdata[16] == 0x03)
				|| (rdataSize == 9 && rdata[0] == 0x02 || rdata[8] == 0x03))
			{
				memcpy(ReadData, &rdata[1], rdataSize - 2);
				*ReadCount = rdataSize - 2;
			}
			else
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "FVEDIO not expected response");
				throw FVEDIOExc(GetCOMPort(), msg);
			}
		}

		void FVEDIOSerial::Write(UINT8 WriteData[], UINT16 WriteCount)
		{
			CAutoLock al(m_FVEDIOSerialLock);

			//Header
			vector<UINT8> wdata(WriteCount + 2);
			unsigned int wdataSize = 0;

			wdata[0] = 0x02;

			int i;
			for (i = 0; i < WriteCount; i++)
			{
				wdata[i + 1] = WriteData[i];
			}
			i += 1;
			wdata[i++] = 0x03;
			wdataSize = i;

			if (wdataSize > 17)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "Message too big >17bytes, actual size: %d bytes", wdataSize);
				throw FVEDIOExc(GetCOMPort(), msg);
			}

#ifdef FVEDIOSERIAL_LOG
			SYSTEMTIME wt;
			GetSystemTime(&wt);
			Log(GetCOMPort(), "W", wdata.data(), wdataSize, wt);
#endif
			try
			{
				Serial::Write(wdata.data(), wdataSize);
			}
			catch (char* &e)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg), "%s", (const char*)e);
				throw FVEDIOExc(GetCOMPort(), msg);
			}
		}
	}
}