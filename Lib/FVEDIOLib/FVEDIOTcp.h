#pragma once

#include "Socket.h"
#include "Lock.h"

using namespace Utilities;

namespace Utilities
{
	namespace FVEDIO
	{
		class FVEDIOTcp : private Socket
		{
		public:
			FVEDIOTcp(void) : Socket() {};
			FVEDIOTcp(const char* IPaddress) : Socket(IPaddress) {};
			~FVEDIOTcp(void) {};

		public:
			void Query(UINT8 WriteData[], UINT16 WriteCount, UINT8 ReadData[], PUINT32 ReadCount);
			void Write(UINT8 WriteData[], UINT16 WriteCount);
			bool IsInitialized(void) { return Socket::IsInitialized(); };
			const char* GetIPaddress() { return Socket::GetIPaddress(); };
			int GetTimeout() { return Socket::GetTimeout(); };

		private:
			CLock m_FVEDIOTcpLock;
		};
	}
}

