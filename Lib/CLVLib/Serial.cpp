#include "stdafx.h"
#include "Serial.h"

namespace Utilities
{
	namespace CLV
	{
#define TIMEOUT 2500
//#define NO_OVERLAPPED

#if !defined(ARRAY_CNT)
#define ARRAY_CNT( arr) sizeof(arr)/sizeof(*arr)
#endif

		static struct {
			DWORD id;
			char *str;
		} gs_comm_evt[] = {
		#define E(id)	EV_##id,	#id
			/* taken from <winbase.h> */
			E(RXCHAR),		// Any Character received
			E(RXFLAG),		// Received certain character
			E(TXEMPTY),		// Transmitt Queue Empty
			E(CTS),			// CTS changed state
			E(DSR),			// DSR changed state
			E(RLSD),		// RLSD (CD) changed state
			E(ERR),			// Line status error occurred
			E(RING),		// Ring signal detected
			E(RX80FULL),	// Receive buffer is 80 percent full
			E(BREAK),		// BREAK received
			E(PERR),		// Printer error occured
			E(EVENT1),		// Provider specific event 1
			E(EVENT2),		// Provider specific event 2
		#undef E
		};

#define COMM_EVTs	ARRAY_CNT(gs_comm_evt)

		Serial::Serial(void)
		{
			m_COMPort = string("COM1");
			m_Timeout = TIMEOUT;
			m_Fh = 0;

			m_Dcb.DCBlength = sizeof(m_Dcb);
			m_Dcb.BaudRate = BR_9600;
			m_Dcb.ByteSize = DB_8;
			m_Dcb.Parity = P_NONE;
			m_Dcb.StopBits = SB_ONE;

			memset(&m_Overlapped, 0, sizeof(m_Overlapped));
			memset(&m_TermCharacter, 0, sizeof(m_TermCharacter));
		}

		Serial::Serial(const char* COMPort, BaudRate Baudrate, DataBits Databits, Parity parity, StopBits Stopbit)
		{
			m_COMPort = string(COMPort);
			m_Timeout = TIMEOUT;
			m_Fh = 0;

			m_Dcb.DCBlength = sizeof(m_Dcb);
			m_Dcb.BaudRate = Baudrate;
			m_Dcb.ByteSize = Databits;
			m_Dcb.Parity = parity;
			m_Dcb.StopBits = Stopbit;

			memset(&m_Overlapped, 0, sizeof(m_Overlapped));
			memset(&m_TermCharacter, 0, sizeof(m_TermCharacter));
		}

		Serial::~Serial(void)
		{
			Disconnect();
		}

		const char* Serial::GetLastErrorString(DWORD dwLastError)
		{
			//DWORD dwLastError = GetLastError(); //call just one time
			WCHAR lpBuffer[2048];

			if (dwLastError != 0)
			{
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					dwLastError,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					lpBuffer,
					wcslen(lpBuffer) - 1,
					NULL);
			}

			wstring wstrBuffer(lpBuffer);

			m_GetLastError.clear();
			m_GetLastError.append(wstrBuffer.begin(), wstrBuffer.end());

			size_t pos = m_GetLastError.find("\r\n");
			if (pos != string::npos)
				m_GetLastError.replace(pos, sizeof("\r\n"), " ");

			return m_GetLastError.c_str();
		}

		void Serial::Connect(const char* COMPort, BaudRate Baudrate, DataBits Databits, Parity parity, StopBits Stopbits)
		{
			char	comport[32];
			HANDLE	fh = NULL;
			DCB		dcb;
			BOOL	succeed;
			COMMTIMEOUTS   	timeouts;
			DWORD	mask;

			strcpy_s(comport, sizeof(comport), COMPort);
			if (!strncmp(COMPort, "COM", strlen("COM")))
			{
				int	port = atoi(COMPort + strlen("COM"));

				if (port > 8)
				{
					sprintf_s(comport, sizeof(comport), "\\\\.\\%s", COMPort);
				}
			}

			m_COMPort = string(comport);

			wstring wcomport(m_COMPort.begin(), m_COMPort.end());
			fh = CreateFile(
				wcomport.c_str(),				// LPCTSTR lpFileName
				GENERIC_READ | GENERIC_WRITE,
				0,						// DWORD dwShareMode
				NULL,					// LPSECURITY_ATTRIBUTES lpSecurityAttributes
				OPEN_EXISTING,			// DWORD dwCreationDisposition
#ifdef NO_OVERLAPPED
				0,						// DWORD dwFlagsAndAttributes
#else
				FILE_FLAG_OVERLAPPED,	// DWORD dwFlagsAndAttributes
#endif
				NULL					// HANDLE hTemplateFile
			);

			if (fh == INVALID_HANDLE_VALUE)
			{
				char err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "CreateFile() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}
			m_Fh = fh;

			dcb.DCBlength = sizeof(dcb);

			succeed = GetCommState(fh, &dcb);
			if (succeed == FALSE)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "GetCommState() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}
			dcb.BaudRate = Baudrate;
			dcb.ByteSize = Databits;
			dcb.Parity = parity;
			dcb.StopBits = Stopbits;
			succeed = SetCommState(fh, &dcb);
			if (succeed == FALSE)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "SetCommState() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}
			m_Dcb = dcb;

			succeed = GetCommTimeouts(fh, &timeouts);
			if (succeed == FALSE)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "SetCommState() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}
			timeouts.ReadIntervalTimeout = MAXDWORD;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.ReadTotalTimeoutConstant = 0;
			timeouts.WriteTotalTimeoutMultiplier = 0;
			timeouts.WriteTotalTimeoutConstant = 0;		/* no timeout */
			succeed = SetCommTimeouts(fh, &timeouts);
			if (succeed == FALSE)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "SetCommState() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}

			int size = 128 * 1024;
			succeed = SetupComm(fh, size, size);
			if (succeed == FALSE)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "SetCommState() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}
			succeed = PurgeComm(fh, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			if (succeed == FALSE)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "SetCommState() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}

#if !defined(NO_OVERLAPPED)
			memset(&m_Overlapped.ReadWriteFile, 0, sizeof(m_Overlapped.ReadWriteFile));
			m_Overlapped.ReadWriteFile.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (!m_Overlapped.ReadWriteFile.hEvent)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "CreateEvent() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}

			memset(&m_Overlapped.WaitCommEvent, 0, sizeof(m_Overlapped.WaitCommEvent));
			m_Overlapped.WaitCommEvent.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (!m_Overlapped.WaitCommEvent.hEvent)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "CreateEvent() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}
			m_Overlapped.on = TRUE;
#endif

			for (int i = mask = 0; i < COMM_EVTs; i++)
				mask |= gs_comm_evt[i].id;

			succeed = SetCommMask(fh, mask);
			if (succeed == FALSE)
			{
				Disconnect();

				char	err_msg[256];
				DWORD dwLastError = GetLastError();
				sprintf_s(err_msg, sizeof(err_msg)-1, "SetCommState() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
				throw err_msg;
			}
		}

		void Serial::Disconnect()
		{
			if (m_Fh)
			{
				CloseHandle(m_Fh);
				m_Fh = 0;

#if !defined(NO_OVERLAPPED)
				if (m_Overlapped.ReadWriteFile.hEvent == 0)
				{
					CloseHandle(m_Overlapped.ReadWriteFile.hEvent);
					m_Overlapped.ReadWriteFile.hEvent = 0;
				}

				if (m_Overlapped.WaitCommEvent.hEvent == 0)
				{
					CloseHandle(m_Overlapped.WaitCommEvent.hEvent);
					m_Overlapped.WaitCommEvent.hEvent = 0;
				}
#endif
			}
		}

		void Serial::Read(UINT8 ReadData[], PUINT32 ReadCount)
		{
			if (!IsConnected())
				Connect(m_COMPort.c_str(), (BaudRate)m_Dcb.BaudRate, (DataBits)m_Dcb.ByteSize, (Parity)m_Dcb.Parity, (StopBits)m_Dcb.StopBits);

			CAutoLock al(m_SerialLock);

			read(ReadData, ReadCount);
		}

		void Serial::read(UINT8 ReadData[], PUINT32 ReadCount)
		{
			if (!m_TermCharacter.set)
			{
				serial_read(ReadData, ReadCount);
			}
			else
			{
				UINT32 ReadDataSize = *ReadCount;
				UINT32 readCountAct = 0, readCount = 0;

				if (ReadCount) *ReadCount = 0;

				clock_t	start_clock = clock();
				do
				{
					readCountAct = ReadDataSize - readCount;
					serial_read(ReadData + readCount, &readCountAct);
					readCount += readCountAct;

					//check termination character at the end of message
					if (ReadData[readCount - 1] == m_TermCharacter.ch)
					{
						if (ReadCount) *ReadCount = readCount;
						break;
					}

					//buffer is full
					if (readCount == ReadDataSize)
					{
						char err_msg[256];
						sprintf_s(err_msg, sizeof(err_msg)-1, "Read() buffer full (%d bytes), no termination char received", ReadDataSize);
						throw err_msg;
					}

					//timeout
					if (((clock() - start_clock) / (double)CLOCKS_PER_SEC) > ((double)m_Timeout / 1000.0))
					{
						char err_msg[256];
						if (readCount == 0)
							sprintf_s(err_msg, sizeof(err_msg)-1, "Read() timeout: %d [ms]; no data received", m_Timeout);
						else
							sprintf_s(err_msg, sizeof(err_msg)-1, "Read() timeout: %d [ms]; no termination char received", m_Timeout);

						throw err_msg;
					}
				} while (true);
			}
		}

		void Serial::Read(UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest)
		{
			if (!IsConnected())
				Connect(m_COMPort.c_str(), (BaudRate)m_Dcb.BaudRate, (DataBits)m_Dcb.ByteSize, (Parity)m_Dcb.Parity, (StopBits)m_Dcb.StopBits);

			CAutoLock al(m_SerialLock);

			read(ReadData, ReadCount, ReadCountRequest);
		}

		void Serial::read(UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest)
		{
			UINT32	ReadDataSize = *ReadCount;
			UINT32	readCount = 0, readCountAct = 0;

			if (ReadCount) *ReadCount = 0;

			clock_t	start_clock = clock();
			do
			{
				UINT32 readCountAct = ReadDataSize - readCount;
				serial_read(ReadData + readCount, &readCountAct);
				readCount += readCountAct;

				//buffer is full
				if (readCount == ReadDataSize)
					break;

				//timeout
				if (((clock() - start_clock) / (double)CLOCKS_PER_SEC) > ((double)m_Timeout / 1000.0))
				{
					char err_msg[256];
					sprintf_s(err_msg, sizeof(err_msg)-1, "Read() timeout: %d [ms]", m_Timeout);
					throw err_msg;
				}
			} while (readCount < ReadCountRequest);

			if (ReadCount) *ReadCount = readCount;
		}

		void Serial::serial_read(UINT8 RadData[], PUINT32 ReadCount)
		{
			DWORD l, le, mask, len = 0;
			UINT32	RadDataSize = *ReadCount;
			BOOL succeed;

			if (ReadCount) *ReadCount = 0;

			do
			{
				succeed = ReadFile(
					m_Fh,
					RadData + len,
					RadDataSize - len,
					&l,
					(m_Overlapped.on) ? &m_Overlapped.ReadWriteFile : NULL);

				len += l;

				if (!succeed)
				{
					le = GetLastError();
					switch (le)
					{
					case ERROR_IO_PENDING:
						ols_wait_for_overlapped(&m_Overlapped.ReadWriteFile, &l, m_Timeout);
						break;
					default:
						{
							char err_msg[256];
							DWORD dwLastError = le;
							sprintf_s(err_msg, sizeof(err_msg)-1, "ReadFile() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
							throw err_msg;
						}
					}
				}

				if(len == 0) //wait for first data
				{
					do
					{
						ols_wait_comm_event(&mask, m_Timeout);
						if (mask & EV_ERR)
						{
							COMSTAT comstat;
							DWORD errflags;

							ClearCommError(m_Fh, &errflags, &comstat);
						}
					} while (!(mask & EV_RXCHAR));
				}
			} while (len == 0);

			if (ReadCount) *ReadCount = l;
		}

		void Serial::Write(UINT8 WriteData[], UINT32 WriteCount)
		{
			if (!IsConnected())
				Connect(m_COMPort.c_str(), (BaudRate)m_Dcb.BaudRate, (DataBits)m_Dcb.ByteSize, (Parity)m_Dcb.Parity, (StopBits)m_Dcb.StopBits);

			CAutoLock al(m_SerialLock);

			serial_write(WriteData, WriteCount);
		}

		void Serial::serial_write(UINT8 WriteData[], UINT32 WriteCount)
		{
			DWORD l;
			BOOL succeed = WriteFile(
				m_Fh,
				WriteData,
				WriteCount,
				&l,
				(m_Overlapped.on) ? &(m_Overlapped.ReadWriteFile) : NULL);

			if (!succeed)
			{
				DWORD le = GetLastError();
				switch (le)
				{
				case ERROR_IO_PENDING:
					ols_wait_for_overlapped(&m_Overlapped.ReadWriteFile, &l, m_Timeout);
					break;
				default:
					{
						char err_msg[256];
						DWORD dwLastError = le;
						sprintf_s(err_msg, sizeof(err_msg)-1, "WriteFile() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
						throw err_msg;
					}
				}
			}
		}

		void Serial::Query(UINT8 WriteData[], UINT32 WriteCount, UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest)
		{
			if (!IsConnected())
				Connect(m_COMPort.c_str(), (BaudRate)m_Dcb.BaudRate, (DataBits)m_Dcb.ByteSize, (Parity)m_Dcb.Parity, (StopBits)m_Dcb.StopBits);

			CAutoLock al(m_SerialLock);

			serial_write(WriteData, WriteCount);

			if (ReadCountRequest == 0)
				read(ReadData, ReadCount);
			else
				read(ReadData, ReadCount, ReadCountRequest);
		}

		void Serial::ols_wait_for_overlapped(LPOVERLAPPED p_overlapped, DWORD *p_len, DWORD timeout)
		{
			BOOL succeed;

			DWORD ret = WaitForSingleObject(p_overlapped->hEvent, timeout == -1 ? INFINITE : timeout);
			switch (ret)
			{
			case WAIT_OBJECT_0:
				succeed = GetOverlappedResult(m_Fh, p_overlapped, p_len, FALSE);
				if (!succeed)
				{
					char err_msg[256];
					DWORD dwLastError = GetLastError();
					sprintf_s(err_msg, sizeof(err_msg)-1, "GetOverlappedResult() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
					throw err_msg;
				}
				break;
			case WAIT_TIMEOUT:
				{
					ResetEvent(p_overlapped->hEvent);

					char err_msg[256];
					sprintf_s(err_msg, sizeof(err_msg)-1, "WaitForSingleObject() timeout: %d [ms]", timeout);
					throw err_msg;
				}
				break;
			case WAIT_FAILED:
				{
					char err_msg[256];
					DWORD dwLastError = GetLastError();
					sprintf_s(err_msg, sizeof(err_msg)-1, "WaitForSingleObject() failed: %d; %s", dwLastError, GetLastErrorString(dwLastError));
					throw err_msg;
				}
				break;
			default:
				{
					char err_msg[256];
					DWORD dwLastError = GetLastError();
					sprintf_s(err_msg, sizeof(err_msg), "WaitForSingleObject() unknown return value: %d; error: %d; %s", ret, dwLastError, GetLastErrorString(dwLastError));
					throw err_msg;
				}
				break;
			}
		}

		void Serial::ols_wait_comm_event(DWORD *pmask, UINT32 timeout)
		{
			DWORD l, le;
			BOOL succeed;
			
			if (pmask) *pmask = 0;

			succeed = WaitCommEvent(
				m_Fh,
				pmask,
				(m_Overlapped.on) ? &m_Overlapped.WaitCommEvent : NULL);

			if (!succeed)
			{
				le = GetLastError();
				switch (le)
				{
				case ERROR_IO_PENDING:
					ols_wait_for_overlapped(&m_Overlapped.WaitCommEvent, &l, timeout);
					break;
				default:
					{
						char err_msg[256];
						DWORD dwLastError = le;
						sprintf_s(err_msg, sizeof(err_msg)-1, "WaitCommEvent() error: %d; %s", dwLastError, GetLastErrorString(dwLastError));
						throw err_msg;
					}
				}
			}
		}
	}
}