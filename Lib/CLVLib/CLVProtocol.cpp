#include "stdafx.h"
#include "CLVProtocol.h"
#include "CLVException.h"

extern char gModuleName[];

namespace Utilities
{
	namespace CLV
	{

#define CLV_THREAD_TIMEOUT				2000
#define CLV_THREAD_LOOP_TIME			100
#define CLV_THREAD_LOOP_TIME_READ_DONE	2000

		void CLVProtocol::ReadingMode()
		{
			UINT8 wdata[16];
			int index = 0;
			
			wdata[index++] = '1';
			wdata[index++] = '1';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if(rdataSize==0 || 0!=strncmp((const char*)rdata, "11", rdataSize))
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: '11'");
				throw CLVExc(GetCOMPort(), msg);
			}	
		}

		void CLVProtocol::PercentageEvaluation()
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = '1';
			wdata[index++] = '2';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if (rdataSize == 0 || 0 != strncmp((const char*)rdata, "12", rdataSize))
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: '12'");
				throw CLVExc(GetCOMPort(), msg);
			}
		}

		void CLVProtocol::Parametrization()
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = '1';
			wdata[index++] = '3';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if (rdataSize == 0 || 0 != strncmp((const char*)rdata, "13", rdataSize))
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: '13'");
				throw CLVExc(GetCOMPort(), msg);
			}
		}

		void CLVProtocol::OperatingData()
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = '1';
			wdata[index++] = '4';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if (rdataSize == 0 || 0 != strncmp((const char*)rdata, "14", rdataSize))
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: '14'");
				throw CLVExc(GetCOMPort(), msg);
			}
		}

		void CLVProtocol::SelfTest(bool waitForResponse)
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = '1';
			wdata[index++] = '5';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				//CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);

				CLVSerial::Write(wdata, wdataSize);

				if (waitForResponse)
				{
					Sleep(6000);
					CLVSerial::Read(rdata, &rdataSize);
				}
				break;
			}

			if (waitForResponse)
			{
				if (rdataSize == 0)
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg)-1, "Response size = 0");
					throw CLVExc(GetCOMPort(), msg);
				}

				int requestId, responseId;
				int cnt = sscanf_s((const char*)rdata, "%02d%03d", &requestId, &responseId);
				if (cnt == 2 && requestId == 15)
				{
					char msg[256];
					switch (responseId)
					{
					case 0:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 0, System OK");
						break;
					case 11:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 11, Data error");
						break;
					case 12:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 12, Address error");
						break;
					case 13:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 13, RAM too small");
						break;
					case 21:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 21, RAM data error");
						break;
					case 22:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 22, RAM address error");
						break;
					case 31:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 31, Register data error");
						break;
					case 32:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 32, Register address error");
						break;
					case 41:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 41, Function test error");
						break;
					case 51:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 51, Speed outside tolerance");
						break;
					case 55:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 55, Fingerprint error");
						break;
					case 61:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 61, Mirror interval outside tolerance");
						break;
					case 71:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 71, Laser shutter error");
						break;
					case 72:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 72, LS PWR error");
						break;
					case 91:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 91, Group error");
						break;
					case 92:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 92, Zero pulse width error");
						break;
					case 93:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 93, No pulse error");
						break;
					case 94:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 94, Motor status error");
						break;
					case 95:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest 95, Zero signal error");
						break;
					default:
						sprintf_s(msg, sizeof(msg)-1, "SelfTest %d, unknown error", responseId);
						break;
					}

					if (responseId > 0)
						throw CLVExc(GetCOMPort(), msg);
				}
				else
				{
					char msg[256];
					sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: '15{data}'");
					throw CLVExc(GetCOMPort(), msg);
				}
			}
		}

		void CLVProtocol::AdjustingMode()
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = '1';
			wdata[index++] = 'J';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if (rdataSize == 0 || 0 != strncmp((const char*)rdata, "1J", rdataSize))
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: '1J'");
				throw CLVExc(GetCOMPort(), msg);
			}
		}

		void CLVProtocol::ShowRAlimits()
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = '1';
			wdata[index++] = 'K';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if (rdataSize == 0 || 0 != strncmp((const char*)rdata, "1K", rdataSize))
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: '1K'");
				throw CLVExc(GetCOMPort(), msg);
			}
		}

		void CLVProtocol::TriggerOn()
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = 'K';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if (rdataSize == 0 || 0 != strncmp((const char*)rdata, "K", rdataSize))
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: 'K'");
				throw CLVExc(GetCOMPort(), msg);
			}
		}

		void CLVProtocol::HostInterfaceRead(UINT8 data[], PUINT32 dataSize)
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = 'K';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if (rdataSize == 0)
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg) - 1, "Response empty");
				throw CLVExc(GetCOMPort(), msg);
			}
			else
			{
				*dataSize = (rdataSize > *dataSize) ? *dataSize:rdataSize;
				memcpy(data, rdata, *dataSize);
			}
		}

		void CLVProtocol::TriggerOff()
		{
			UINT8 wdata[16];
			int index = 0;

			wdata[index++] = 'I';

			UINT16 wdataSize = index;

			UINT8 rdata[128];
			unsigned int rdataSize = sizeof(rdata);
			memset(rdata, 0, sizeof(rdata));

			switch (m_Type)
			{
			case CLVProtocolTCP:
				CLVTcp::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			case CLVProtocolSerial:
				CLVSerial::Query(wdata, wdataSize, rdata, &rdataSize);
				break;
			}

			if (rdataSize == 0 || 0 != strncmp((const char*)rdata, "I", rdataSize))
			{
				char msg[256];
				sprintf_s(msg, sizeof(msg)-1, "Response wrong format, expected: 'I'");
				throw CLVExc(GetCOMPort(), msg);
			}
		}

		void CLVProtocol::StartReading(bool WaitForTaskRunning, UINT32 WaitAfterReadDone)
		{
			if (m_hThread == 0)
			{
				m_LoopTimeMS = CLV_THREAD_LOOP_TIME;
				m_WaitAfterReadDone = (WaitAfterReadDone == 0) ? CLV_THREAD_LOOP_TIME_READ_DONE : WaitAfterReadDone;

				m_StopEvent = CreateEvent(NULL,   // No security attributes
					TRUE,  // Manual-reset event
					FALSE,   // Initial state is not signaled
					NULL);  // Object not named

				m_hThread = CreateThread(NULL, 0, &CLVProtocol::ThreadFunc, this, 0, &m_threadID);

				if (WaitForTaskRunning)
				{
					this->WaitForTaskRunning();
				}
			}
		}

		void CLVProtocol::StopReading()
		{
			if (m_StopEvent)
			{
				SetEvent(m_StopEvent);

				int timeout = CLV_THREAD_TIMEOUT;

				if (WaitForSingleObject(m_hThread, timeout))
				{
					//timeout
					if (m_TaskIsRunning)
					{
						TerminateThread(m_hThread, 0);
						m_TaskIsRunning = false;
					}
				}
				m_threadID = 0;
				CloseHandle(m_StopEvent);
				m_StopEvent = 0;
				CloseHandle(m_hThread);
				m_hThread = 0;
			}
		}

		void CLVProtocol::WaitForTaskRunning()
		{
			int timeout = CLV_THREAD_TIMEOUT;
			clock_t t = clock();
			UINT32 wait = ((m_LoopTimeMS / 2) > 2) ? (m_LoopTimeMS / 2) : 2;
			while (!m_TaskIsRunning)
			{
				UINT32 elapsed = (UINT32)(clock() - t);
				if (elapsed > (UINT32)timeout)
				{
					char msg[512];
					sprintf_s(msg, sizeof(msg)-1, "WaitForTaskRunning() error: timeout expired %d[ms]", elapsed);
					throw CLVExc(GetCOMPort(), msg);
				}
				Sleep(wait);
			}
		}

		void CLVProtocol::LogFile(char msg[])
		{
			//log file
			string spath(gModuleName);
			spath.replace(spath.find(".dll"), sizeof(".txt") - 1, ".txt");

			FILE* log = fopen(spath.data(), "a");
			if (log != NULL)
			{
				char stime[64];
				time_t now = time(NULL);
				struct tm *t = localtime(&now);
				strftime(stime, sizeof(stime) - 1, "[%Y.%m.%d %H:%M:%S]", t);

				fprintf_s(log, "%s %s\n", stime, msg);
				fclose(log);
			}
		}

		DWORD WINAPI CLVProtocol::ThreadFunc(void* pArguments)
		{
			CLVProtocol* me = (CLVProtocol*)pArguments;
			UINT32 wait = -1;
			clock_t t;
			UINT32 wait_default = me->m_LoopTimeMS; //ms default 20ms

			memset(me->m_Barcode, 0, sizeof(me->m_Barcode));
			UINT8 rdata[256];
			bool read_done;

			do
			{
				t = clock();

				try
				{
					//Read
					unsigned int rdataSize = sizeof(rdata);
					memset(rdata, 0, sizeof(rdata));
					read_done = false;

#if defined(INTERFACE_TERMINAL)
					me->TriggerOn();

					//read data 
					//if no code - BR return NO DATA in 2000
					//if no read - exception TIMEOUT 2500
					me->Read(rdata, &rdataSize);  
					
					me->TriggerOff();
						
					if (rdataSize != 0)
					{
						CAutoLock al(me->m_Lock);
						memset(me->m_Barcode, 0, sizeof(me->m_Barcode));
						memcpy(me->m_Barcode, rdata, (rdataSize < sizeof(me->m_Barcode)-1) ? rdataSize : sizeof(me->m_Barcode)-1);


						//<STX>TT = _5000ms MG = _76 % n = _0 AK = 1 <0D><0A> no  code !<0D><0A><0D><0A><0A><0D><ETX>
						//<STX>TT = _4840ms MG = _76 % n = _1 AK = 1 <0D><0A>BX80677I57600<0D><0A>C39  100 % ST = 0 CP = _47 CL = 13 CA = _16 CS = __6 CK = _16 DI = F<0D><0A><0A><0D><ETX>

#define CLV_HOST_TRANSMIT_FAULT "  host transmit fault "
#define CLV_NO_CODE				" no  code !"

						char *next_token = NULL;
						char *token = strtok_s((char*)rdata, "\n", &next_token);
						if (next_token != NULL)
						{
							token = strtok_s(next_token, "\r", &next_token);

							if (token != NULL 
								&& 0 != strncmp(token, CLV_HOST_TRANSMIT_FAULT, strlen(CLV_HOST_TRANSMIT_FAULT))
								&& 0 != strncmp(token, CLV_NO_CODE, strlen(CLV_NO_CODE))
								)
							{
								me->OnValueChanged((const char*)token, strlen(token));
								read_done = true;
							}
							else if (0 == strncmp(token, CLV_HOST_TRANSMIT_FAULT, strlen(CLV_HOST_TRANSMIT_FAULT)))
							{
								LogFile(CLV_HOST_TRANSMIT_FAULT);
							}
						}
					}
#else //INTERFACE_HOST
					me->HostInterfaceRead(rdata, &rdataSize);

					if (rdataSize != 0)
					{
						CAutoLock al(me->m_Lock);
						memset(me->m_Barcode, 0, sizeof(me->m_Barcode));
						memcpy(me->m_Barcode, rdata, (rdataSize < sizeof(me->m_Barcode) - 1) ? rdataSize : sizeof(me->m_Barcode) - 1);

#define CLV_NO_CODE		"nocode"

						if (0 != strncmp(me->m_Barcode, CLV_NO_CODE, strlen(CLV_NO_CODE)))
						{
							me->OnValueChanged((const char*)me->m_Barcode, strlen(me->m_Barcode));
							read_done = true;
						}
					}
#endif
				}
				catch (CLVException &e)
				{
					char msg[1024];
					e.str(msg, sizeof(msg));
					//printf("%s\n", msg);

					//ignore timeout
					bool log_timeout = true;
					string smsg(msg);
					size_t pos = smsg.find("timeout", 0);
					if (log_timeout || (!log_timeout && pos == string::npos))
					{
						LogFile(msg);
					}
				}

				if (!me->m_TaskIsRunning)
					me->m_TaskIsRunning = true;

				//calculate next wait
				if(read_done)
				{
					wait = me->m_WaitAfterReadDone;
				}
				else
				{
					UINT32 elapsed = (UINT32)(clock() - t);
					if (wait_default > elapsed)
						wait = wait_default - elapsed;
					else
					{
#ifdef _DEBUG
						if (elapsed > (wait_default*1.5))
							printf("Task timeout: %d [ms]; default: %d [ms]\n", elapsed, wait_default);
#endif
						wait = 1;
					}
				}

			} while (WaitForSingleObject(me->m_StopEvent, wait) != WAIT_OBJECT_0);
			
			me->m_TaskIsRunning = false;

			return 0;
		}

		void CLVProtocol::GetBarcode(char data[], UINT32 dataSize)
		{
			CAutoLock al(m_Lock);
			memset(data, 0, dataSize);
			memcpy(data, m_Barcode, (dataSize > sizeof(m_Barcode)) ? sizeof(m_Barcode) : dataSize);
			memset(m_Barcode, 0, sizeof(m_Barcode));
		}
	}


}