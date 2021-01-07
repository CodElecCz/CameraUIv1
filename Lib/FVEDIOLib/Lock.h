#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

namespace Utilities
{
	namespace FVEDIO
	{
		class CLock
		{
		public:
			CLock(void) { InitializeCriticalSection(&m_Cs); };
			~CLock(void) { DeleteCriticalSection(&m_Cs); };

			void lock(void) { EnterCriticalSection(&m_Cs); };
			void unlock(void) { LeaveCriticalSection(&m_Cs); };

		private:
			CRITICAL_SECTION m_Cs;
		};


		class CAutoLock
		{
		public:
			CAutoLock(CLock &arg) : m_Lock(arg) { m_Lock.lock(); }
			~CAutoLock() { m_Lock.unlock(); }
		protected:
			CLock &m_Lock;
		};

		class CAutoUnlock
		{
		public:
			CAutoUnlock(CLock &arg) : m_Lock(arg) { m_Lock.unlock(); }
			~CAutoUnlock() { m_Lock.lock(); }

		protected:
			CLock &m_Lock;
		};
	}
}