#include "stdafx.h"
#include "Socket.h"

namespace Utilities
{
	namespace CLV
	{
#define TIMEOUT 1000

		Socket::Socket(void)
		{
			m_Socket = INVALID_SOCKET;
			m_IPaddress = string("127.0.0.1");
			m_Port = 502;
			m_Timeout = TIMEOUT;
			m_WSAStartup = false;

			WSADATA wsadata;

			if (WSAStartup(MAKEWORD(2, 2), &wsadata))
			{
				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "WSAStartup() error");
				throw err_msg;
			}
			else
				m_WSAStartup = true;
		}

		Socket::Socket(const char* IPaddress, UINT16 port)
		{
			m_Socket = INVALID_SOCKET;
			m_IPaddress = string(IPaddress);
			m_Port = port;
			m_Timeout = TIMEOUT;
			m_WSAStartup = false;

			WSADATA wsadata;

			if (WSAStartup(MAKEWORD(2, 2), &wsadata))
			{
				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "WSAStartup() error");
				throw err_msg;
			}
			else
				m_WSAStartup = true;
		}

		Socket::~Socket(void)
		{
			if (m_WSAStartup)
			{
				Disconnect();
				WSACleanup();
				m_WSAStartup = false;
			}
		}

		typedef struct SWSAErrorList {
			int num;
			const char *sym;
			const char *str;
		} WSAErrorList;

		static WSAErrorList m_WSAErrorList[] = {
				-1,					"WSA",
									NULL,
				WSAEACCES,			"EACCES",
									"Permission denied",
				WSAEADDRINUSE,		"EADDRINUSE",
									"Address already in use",
				WSAEADDRNOTAVAIL,	"EADDRNOTAVAIL",
									"Cannot assign requested address",
				WSAEAFNOSUPPORT,	"EAFNOSUPPORT",
									"Address family not supported by protocol family",
				WSAEALREADY,		"EALREADY",
									"Operation already in progress",
				WSAECONNABORTED,	"ECONNABORTED",
									"Software caused connection abort",
				WSAECONNREFUSED,	"ECONNREFUSED",
									"Connection refused",
				WSAECONNRESET,		"ECONNRESET",
									"Connection reset by peer",
				WSAEDESTADDRREQ,	"EDESTADDRREQ",
									"Destination address required",
				WSAEFAULT,			"EFAULT",
									"Bad address",
				WSAEHOSTDOWN,		"EHOSTDOWN",
									"Host is down",
				WSAEHOSTUNREACH,	"EHOSTUNREACH",
									"No route to host",
				WSAEINPROGRESS,		"EINPROGRESS",
									"Operation now in progress",
				WSAEINTR,			"EINTR",
									"Interrupted function call",
				WSAEINVAL,			"EINVAL",
									"Invalid argument",
				WSAEISCONN,			"EISCONN",
									"Socket is already connected",
				WSAEMFILE,			"EMFILE",
									"Too many open files",
				WSAEMSGSIZE,		"EMSGSIZE",
									"Message too long",
				WSAENETDOWN,		"ENETDOWN",
									"Network is down",
				WSAENETRESET,		"ENETRESET",
									"Network dropped connection on reset",
				WSAENETUNREACH,		"ENETUNREACH",
									"Network is unreachable",
				WSAENOBUFS,			"ENOBUFS",
									"No buffer space available",
				WSAENOPROTOOPT,		"ENOPROTOOPT",
									"Bad protocol option",
				WSAENOTCONN,		"ENOTCONN",
									"Socket is not connected",
				WSAENOTSOCK,		"ENOTSOCK",
									"Socket operation on non-socket",
				WSAEOPNOTSUPP,		"EOPNOTSUPP",
									"Operation not supported",
				WSAEPFNOSUPPORT,	"EPFNOSUPPORT",
									"Protocol family not supported",
				WSAEPROCLIM,		"EPROCLIM",
									"Too many processes",
				WSAEPROTONOSUPPORT,	"EPROTONOSUPPORT",
									"Protocol not supported",
				WSAEPROTOTYPE,		"EPROTOTYPE",
									"Protocol wrong type for socket",
				WSAESHUTDOWN,		"ESHUTDOWN",
									"Cannot send after socket shutdown",
				WSAESOCKTNOSUPPORT,	"ESOCKTNOSUPPORT",
									"Socket type not supported",
				WSAETIMEDOUT,		"ETIMEDOUT",
									"Connection timed out",
				WSAEWOULDBLOCK,		"EWOULDBLOCK",
									"Resource temporarily unavailable",
				WSAHOST_NOT_FOUND,	"HOST_NOT_FOUND",
									"Host not found",
//				WSA_INVALID_HANDLE,	"_INVALID_HANDLE",
//									"Specified event object handle is invalid",
//				WSA_INVALID_PARAMETER,	"_INVALID_PARAMETER",
//									"One or more parameters are invalid",
				WSAEINVALIDPROCTABLE,	"EINVALIDPROCTABLE",
									"Invalid procedure table from service provider",
				WSAEINVALIDPROVIDER,"EINVALIDPROVIDER",
									"Invalid service provider version number",
//				WSA_IO_PENDING,		"_IO_PENDING",
//									"Overlapped operations will complete later",
//				WSA_IO_INCOMPLETE,	"_IO_INCOMPLETE",
//									"Overlapped I/O event object not in signaled state",
//				WSA_NOT_ENOUGH_MEMORY,	"_NOT_ENOUGH_MEMORY",
//									"Insufficient memory available",
				WSANOTINITIALISED,	"NOTINITIALISED",
									"Successful WSAStartup not yet performed",
				WSANO_DATA,			"NO_DATA",
									"Valid name, no data record of requested type",
				WSANO_RECOVERY,		"NO_RECOVERY",
									"This is a non-recoverable error",
				WSAEPROVIDERFAILEDINIT,	"EPROVIDERFAILEDINIT",
									"Unable to initialize a service provider",
				WSASYSCALLFAILURE,	"SYSCALLFAILURE",
									"System call failure",
				WSASYSNOTREADY,		"SYSNOTREADY",
									"Network subsystem is unavailable",
				WSATRY_AGAIN,		"TRY_AGAIN",
									"Non-authoritative host not found",
				WSAVERNOTSUPPORTED,	"VERNOTSUPPORTED",
									"WINSOCK.DLL version out of range",
				WSAEDISCON,			"EDISCON",
									"Graceful shutdown in progress",
//				WSA_OPERATION_ABORTED,	"_OPERATION_ABORTED",
//									"Overlapped operation aborted",
		};

		const char* WSAGetLastErrorString(int error)
		{
			const char* serror = "WSAErrorList unknown error";
			int size = sizeof(m_WSAErrorList) / sizeof(m_WSAErrorList[0]);
			for (int i = 0; i < size; i++)
			{
				if (m_WSAErrorList[i].num == error)
				{
					serror = m_WSAErrorList[i].str;
					break;
				}
			}

			return serror;
		}

		const char* WSAGetLastErrorSymbol(int error)
		{
			const char* serror = "WSAErrorList unknown error";
			int size = sizeof(m_WSAErrorList) / sizeof(m_WSAErrorList[0]);
			for (int i = 0; i < size; i++)
			{
				if (m_WSAErrorList[i].num == error)
				{
					serror = m_WSAErrorList[i].sym;
					break;
				}
			}

			return serror;
		}

		void Socket::Connect(const char* IPaddress, UINT16 port)
		{
			m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
			if (m_Socket == INVALID_SOCKET)
			{
				int wsa_err = WSAGetLastError();
				const char* wsa_msg = WSAGetLastErrorString(wsa_err);
				const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "socket() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);
				throw err_msg;
			}

			//set the socket in non-blocking
			unsigned long iMode = 1;
			int iResult = ioctlsocket(m_Socket, FIONBIO, &iMode);
			if (iResult == SOCKET_ERROR)
			{
				Disconnect();

				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "ioctlsocket(FIONBIO) error: %d", iResult);
				throw err_msg;
			}

			//Fill out the information needed to initialize a socket…
			SOCKADDR_IN target; //Socket address information

			target.sin_family = AF_INET; // address family Internet
			target.sin_port = htons(port); //Port to connect on
			target.sin_addr.s_addr = inet_addr(IPaddress); //Target IP

			iResult = connect(m_Socket, (SOCKADDR *)&target, sizeof(target));
			if (iResult == SOCKET_ERROR)
			{
				int wsa_err = WSAGetLastError();
				if (wsa_err == WSAEWOULDBLOCK)
				{
					// restart the socket mode
					iMode = 0;
					iResult = ioctlsocket(m_Socket, FIONBIO, &iMode);
					if (iResult == SOCKET_ERROR)
					{
						Disconnect();

						char err_msg[256];
						sprintf_s(err_msg, sizeof(err_msg), "ioctlsocket(FIONBIO) error: %d", iResult);
						throw err_msg;
					}

					fd_set Write, Err;
					FD_ZERO(&Write);
					FD_ZERO(&Err);
					FD_SET(m_Socket, &Write);
					FD_SET(m_Socket, &Err);

					// check if the socket is ready
					TIMEVAL Timeout;
					Timeout.tv_sec = m_Timeout / 1000;
					Timeout.tv_usec = (long)(m_Timeout % 1000) * 1000;
					iResult = select(0, NULL, &Write, &Err, &Timeout);
					if (iResult == 0)
					{
						Disconnect();

						char err_msg[256];
						sprintf_s(err_msg, sizeof(err_msg), "connect() timeout %d.%03d sec", Timeout.tv_sec, Timeout.tv_usec / 1000);
						throw err_msg;
					}
					else
					{
						if (FD_ISSET(m_Socket, &Write))
						{
							//SUCCEED
							m_IPaddress = string(IPaddress);

							//receive timeout set
							int timeout = m_Timeout;
							iResult = setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
							if (iResult == SOCKET_ERROR)
							{
								int wsa_err = WSAGetLastError();
								const char* wsa_msg = WSAGetLastErrorString(wsa_err);
								const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

								char err_msg[256];
								sprintf_s(err_msg, sizeof(err_msg), "setsockopt(SO_RCVTIMEO) error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);
								throw err_msg;
							}
						}
						if (FD_ISSET(m_Socket, &Err))
						{
							Disconnect();

							int wsa_errlen = sizeof(wsa_err);
							iResult = getsockopt(m_Socket, SOL_SOCKET, SO_ERROR, (char*)&wsa_err, &wsa_errlen);

							const char* wsa_msg = WSAGetLastErrorString(wsa_err);
							const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

							char err_msg[256];
							sprintf_s(err_msg, sizeof(err_msg), "connect() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);
							throw err_msg;
						}
					}
				}
				else
				{
					Disconnect();

					const char* wsa_msg = WSAGetLastErrorString(wsa_err);
					const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

					char err_msg[256];
					sprintf_s(err_msg, sizeof(err_msg), "connect() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);
					throw err_msg;
				}
			}
		}

		void Socket::Disconnect()
		{
			if (m_Socket != INVALID_SOCKET)
			{
				closesocket(m_Socket);
				m_Socket = INVALID_SOCKET;
			}
		}

		void Socket::socket_send(UINT8 WriteData[], UINT32 WriteCount)
		{
			//send
			int iResult = send(m_Socket, (const char*)WriteData, WriteCount, 0);
			if (iResult == SOCKET_ERROR)
			{
				int wsa_err = WSAGetLastError();
				const char* wsa_msg = WSAGetLastErrorString(wsa_err);
				const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

				char err_msg[256];
				sprintf_s(err_msg, sizeof(err_msg), "send() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);
				throw err_msg;
			}
		}

		void Socket::Send(UINT8 WriteData[], UINT32 WriteCount)
		{
			CAutoLock al(m_SocketLock);

			if (!IsConnected())
				Connect(m_IPaddress.c_str(), m_Port);

			//send
			socket_send(WriteData, WriteCount);
		}

		void Socket::socket_receive(UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest)
		{
			unsigned int ReadCountAlloc = *ReadCount;
			if (ReadCount) *ReadCount = 0;

			//receive
			unsigned char rdata[512];
			unsigned int readCount = 0;
			int iResult;
			do
			{
				iResult = recv(m_Socket, (char*)rdata, sizeof(rdata), 0);
				if (iResult == SOCKET_ERROR)
				{
					int wsa_err = WSAGetLastError();
					const char* wsa_msg = WSAGetLastErrorString(wsa_err);
					const char* wsa_sym = WSAGetLastErrorSymbol(wsa_err);

					char err_msg[256];
					sprintf_s(err_msg, sizeof(err_msg), "recv() error: 0x%X; %s; %s", wsa_err, wsa_sym, wsa_msg);

					Disconnect();

					throw err_msg;
				}
				else if (iResult == 0)
				{
					char err_msg[256];
					sprintf_s(err_msg, sizeof(err_msg), "recv() no data");
					throw err_msg;
				}
				else
				{
					if (readCount < ReadCountAlloc)
					{
						int restOfReadBuffer = ReadCountAlloc - readCount;
						int sizeToCopy = (restOfReadBuffer < iResult) ? restOfReadBuffer : iResult;
						memcpy(&ReadData[readCount], rdata, sizeToCopy);
					}
					readCount += iResult;

					if (ReadCount) *ReadCount = readCount;
				}
			} while (ReadCountRequest > readCount);

			if (ReadCount) *ReadCount = readCount;
		}

		void Socket::Receive(UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest)
		{
			CAutoLock al(m_SocketLock);

			if (!IsConnected())
				Connect(m_IPaddress.c_str(), m_Port);

			//send
			socket_receive(ReadData, ReadCount, ReadCountRequest);
		}

		void Socket::Query(UINT8 WriteData[], UINT32 WriteCount, UINT8 ReadData[], PUINT32 ReadCount, UINT32 ReadCountRequest)
		{
			CAutoLock al(m_SocketLock);

			if (!IsConnected())
				Connect(m_IPaddress.c_str(), m_Port);

			//send
			socket_send(WriteData, WriteCount);

			//receive
			socket_receive(ReadData, ReadCount, ReadCountRequest);
		}
	}
}
	