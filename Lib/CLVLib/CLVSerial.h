#pragma once

#include "Serial.h"
#include "Lock.h"

using namespace Utilities;

namespace Utilities
{
	namespace CLV
	{
		class CLVSerial : public Serial
		{
		public:
			CLVSerial(void) : Serial() {};
			CLVSerial(const char* COMPort, Serial::BaudRate Baudrate = Serial::BR_57600, Serial::DataBits Databits = Serial::DB_8, Serial::Parity Parity = Serial::P_NONE, Serial::StopBits Stopbit = Serial::SB_ONE)
				: Serial(COMPort, Baudrate, Databits, Parity, Stopbit) {};
			~CLVSerial(void) {};

		public:
			void Query(UINT8 WriteData[], UINT16 WriteCount, UINT8 ReadData[], PUINT32 ReadCount);
			void Read(UINT8 ReadData[], PUINT32 ReadCount);
			void Write(UINT8 WriteData[], UINT16 WriteCount);
			bool IsInitialized(void) { return Serial::IsInitialized(); };
			const char* GetCOMPort() { return Serial::GetCOMPort(); };
			int GetTimeout() { return Serial::GetTimeout(); };

		private:
			CLock m_CLVSerialLock;
		};
	}
}

