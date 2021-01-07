#pragma once

//#include <WinSock2.h>
#include <winsock.h>
#include "Lock.h"

using namespace std;

namespace Utilities
{
	namespace CLV
	{
		class Socket
		{
		public:
			Socket(void);
			Socket(const char* IPadress, UINT16 port = 502);
			~Socket(void);

		public:
			bool IsInitialized(void) { return m_WSAStartup; };

			void Connect(const char* IPaddress, UINT16 port = 502);
			bool IsConnected() { return m_Socket != INVALID_SOCKET; };
			void Disconnect();

			void Send(UINT8 WriteData[], UINT32 WriteCount);
			void Receive(UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest); /* Receive until ReadCountRequest or Timeout */
			void Query(UINT8 WriteData[], UINT32 WriteCount, UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest = 0); /* ReadCountRequest==0 read any; ReadCountRequest!=0 read requested */

			const char* GetIPaddress() { return m_IPaddress.c_str(); };
			int GetTimeout() { return m_Timeout; };

		private:
			void socket_send(UINT8 WriteData[], UINT32 WriteCount);
			void socket_receive(UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest);

		private:
			SOCKET m_Socket;
			bool m_WSAStartup;
			string m_IPaddress;
			UINT16 m_Port;
			int m_Timeout;
			CLock m_SocketLock;
		};
	}
}
