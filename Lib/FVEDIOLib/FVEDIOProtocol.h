#pragma once

#include "FVEDIODef.h"
#include "FVEDIOSerial.h"
#include "FVEDIOTCP.h"

using namespace Utilities::FVEDIO;

namespace Utilities
{
	namespace FVEDIO
	{
#pragma warning(push)
#pragma warning(disable:4275)

		enum FVEDIOProtocolType
		{
			FVEDIOProtocolTCP,
			FVEDIOProtocolSerial
		};

		enum FVEDIOLED {
			FVEDIODO = 0x3c,
			FVEDIOLED0 = 0x44,
			FVEDIOLED1 = 0x48,
			FVEDIOLED2 = 0x4c,
			FVEDIOLED3 = 0x50
		};

		class FVEDIO_API FVEDIOProtocol : private FVEDIOSerial, FVEDIOTcp
		{
		public:
			FVEDIOProtocol(const char* IPaddress)
				: FVEDIOTcp(IPaddress), m_Type(FVEDIOProtocolTCP) {};
			FVEDIOProtocol(const char* COMPort, Serial::BaudRate Baudrate, Serial::DataBits Databits = Serial::DB_8, Serial::Parity Parity = Serial::P_NONE, Serial::StopBits Stopbit = Serial::SB_ONE)
				: FVEDIOSerial(COMPort, Baudrate, Databits, Parity, Stopbit), m_Type(FVEDIOProtocolSerial) {};
			~FVEDIOProtocol(void) {};

		public:
			void LED0(UINT8 intensityPercent) { LED(FVEDIOLED0, intensityPercent); };
			void LED1(UINT8 intensityPercent) { LED(FVEDIOLED1, intensityPercent); };
			void LED2(UINT8 intensityPercent) { LED(FVEDIOLED2, intensityPercent); };
			void LED3(UINT8 intensityPercent) { LED(FVEDIOLED3, intensityPercent); };

			void WriteDO(UINT16 data);
			void WriteSingleDO(UINT8 pos, UINT8 val);
			void ReadDO(PUINT16 data);
			
		private:
			void Read4B(UINT16 address, BYTE data[4]);
			void Write4B(UINT16 address, BYTE data[4]);
			void LED(FVEDIOLED index, UINT8 intensityPercent);

		private:
			FVEDIOProtocolType m_Type;
		};
#pragma warning(pop)
	}
}


