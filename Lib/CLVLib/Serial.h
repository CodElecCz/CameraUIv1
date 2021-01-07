#pragma once

#include <xstring>
#include "Lock.h"

using namespace std;

namespace Utilities
{	
	namespace CLV
	{
		class Serial
		{
		
		public: 
			enum BaudRate {
				BR_110 = CBR_110,
				BR_300 = CBR_300,
				BR_600 = CBR_600,
				BR_1200 = CBR_1200,
				BR_2400 = CBR_2400,
				BR_4800 = CBR_4800,
				BR_9600 = CBR_9600,
				BR_14400 = CBR_14400,
				BR_19200 = CBR_19200,
				BR_38400 = CBR_38400,
				BR_56000 = CBR_56000,
				BR_57600 = CBR_57600,
				BR_115200 = CBR_115200,
				BR_128000 = CBR_128000,
				BR_256000 = CBR_256000
			};
			enum DataBits {
				DB_7 = 7,
				DB_8 = 8
			};
			enum Parity {
				P_NONE = NOPARITY,
				P_ODD = ODDPARITY,
				P_EVEN = EVENPARITY,
				P_MARK = MARKPARITY,
				P_SPACE = SPACEPARITY
			};
			enum StopBits {
				SB_ONE = ONESTOPBIT,
				SB_ONE_5 = ONE5STOPBITS,
				SB_TWO = TWOSTOPBITS
			};

		public:
			Serial(void);
			Serial(const char* COMPort, BaudRate Baudrate = BR_57600, DataBits Databits = DB_8, Parity Parity = P_NONE, StopBits Stopbit = SB_ONE);
			~Serial(void);

		public:
			bool IsInitialized(void) { return m_Fh != 0; };

			void Connect(const char* COMPort, BaudRate Baudrate = BR_9600, DataBits Databits = DB_8, Parity Parity = P_NONE, StopBits Stopbit = SB_ONE);
			bool IsConnected() { return m_Fh != 0; };
			void Disconnect();

			void Read(UINT8 ReadData[], PUINT32 ReadCount); /* Read any data or termination character found */
			void Read(UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest); /* Read until ReadCountRequest or Timeout */
			void Write(UINT8 WriteData[], UINT32 WriteCount);
			void Query(UINT8 WriteData[], UINT32 WriteCount, UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest = 0);

			const char* GetCOMPort() { return m_COMPort.c_str(); };
			int GetTimeout() { return m_Timeout; };
			void SetTermChar(BOOL set, UINT8 ch) { m_TermCharacter.set = set; ch = m_TermCharacter.ch = ch; };

		private:
			void ols_wait_comm_event(DWORD *pmask, UINT32 timeout);
			void ols_wait_for_overlapped(LPOVERLAPPED poverlapped, DWORD *p_len, DWORD timeout);
			void serial_read(UINT8 ReadData[], PUINT32 ReadCount); /* ReadFile() */
			void serial_write(UINT8 WriteData[], UINT32 WriteCount); /* WriteFile() */
			void read(UINT8 ReadData[], PUINT32 ReadCount);
			void read(UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest);
			const char * GetLastErrorString(DWORD dwLastError);

		private:
			HANDLE	m_Fh;
			string	m_COMPort;
			string  m_GetLastError;
			int		m_Timeout;
			CLock	m_SerialLock;
			DCB		m_Dcb;

			struct {
				BOOL on;	/* overlapped I/O enabled/disabled */
				OVERLAPPED ReadWriteFile;
				OVERLAPPED WaitCommEvent;
			} m_Overlapped;

			struct {
				BOOL set;
				UINT8 ch;
			} m_TermCharacter;
		};
	}
}
