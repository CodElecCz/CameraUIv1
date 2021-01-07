#pragma once

#include "CLVException.h"
#include "CLVDef.h"
#include "CLVSerial.h"
#include "CLVTCP.h"
#include "events.h"

using namespace Utilities::CLV;

namespace Utilities
{
	namespace CLV
	{
#pragma warning(push)
#pragma warning(disable:4275)

		enum CLVProtocolType
		{
			CLVProtocolTCP,
			CLVProtocolSerial
		};

		class CLV_API CLVProtocol : public CLVSerial, CLVTcp
		{
		public:
			CLVProtocol(const char* IPaddress)
				: CLVTcp(IPaddress), 
				m_Type(CLVProtocolTCP), 
				OnValueChanged(this) 
			{};
			CLVProtocol(const char* COMPort, Serial::BaudRate Baudrate, Serial::DataBits Databits = Serial::DB_8, Serial::Parity Parity = Serial::P_NONE, Serial::StopBits Stopbit = Serial::SB_ONE)
				: CLVSerial(COMPort, Baudrate, Databits, Parity, Stopbit), 
				m_Type(CLVProtocolSerial), 
				m_hThread(0), 
				m_StopEvent(0), 
				m_threadID(0), 
				m_TaskIsRunning(false), 
				m_WaitAfterReadDone(2000), 
				OnValueChanged(this) 
			{};
			~CLVProtocol(void) 
			{
				StopReading();
			};

		public:
			/*
			<STX>11<ETX> Reading Mode			- response <STX>11<ETX>
			<STX>12<ETX> Percentage Evaluation	- response <STX>12<ETX>
			<STX>13<ETX> Parametrization		- response <STX>13<ETX>
			<STX>14<ETX> Operating Data			- response <STX>14<ETX>
			<STX>15<ETX> SelfTest				- response <STX>15000<ETX> 6s

			<STX>1J<ETX> Adjusting Mode			- response <STX>1J<ETX>
			<STX>1K<ETX> Show RA-limits			- response <STX>1K<ETX>

			<STX>K<ETX> Trigger ON 5s	- response <STX>K<ETX> or 21
			<STX>I<ETX> Trigger OFF		- response <STX>K<ETX> or 22
			*/
			void ReadingMode();				//11
			void PercentageEvaluation();	//12
			void Parametrization();			//13
			void OperatingData();			//14
			void SelfTest(bool waitForResponse = true);	//15
			void AdjustingMode();			//1J
			void ShowRAlimits();			//1K
			void TriggerOn();				//K
			void TriggerOff();				//I

			void HostInterfaceRead(UINT8 data[], PUINT32 dataSize);

			
			/*
			<STX>TT=_5000ms MG=_76% n=_0 AK=1<0D><0A> no  code !<0D><0A><0D><0A><0A><0D><ETX>
			<STX>TT=_4840ms MG=_76% n=_1 AK=1<0D><0A>BX80677I57600<0D><0A>C39  100% ST=0 CP=_47 CL=13 CA=_16 CS=__6 CK=_16 DI=F<0D><0A><0A><0D><ETX>
			*/
			void StartReading(bool WaitForTaskRunning = true, UINT32 WaitAfterReadDone = 2000);
			void StopReading();
			bool IsReading() { return m_TaskIsRunning; }
			void GetBarcode(char data[], UINT32 dataSize);
			static void LogFile(char data[]);
			
		private:
			static DWORD WINAPI ThreadFunc(void* pArguments);
			void WaitForTaskRunning();

		public:
#pragma warning(push)
#pragma warning(disable:4251)
			event<CLVProtocol, const char*, UINT64> OnValueChanged;
#pragma warning(pop)

		private:
			CLVProtocolType		m_Type;

			//thread variable
#pragma warning(push)
#pragma warning(disable:4251)
			CLock				m_Lock;
#pragma warning(pop)
			HANDLE				m_hThread;
			HANDLE				m_StopEvent;
			DWORD				m_threadID;
			UINT32				m_LoopTimeMS;
			UINT32				m_WaitAfterReadDone;
			bool				m_TaskIsRunning;
			char				m_Barcode[1024];
		};
#pragma warning(pop)
	}
}


