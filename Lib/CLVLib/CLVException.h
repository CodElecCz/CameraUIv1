#pragma once

#include <exception>
#include <memory>
#include <string>

using namespace std;

namespace Utilities
{
	namespace CLV
	{
#define CLVExc(id, msg) CLVException(id, (const char*)msg, __FILE__, __LINE__, __FUNCTION__)


		class CLVException : public exception
		{
		public:
			CLVException()
				: m_Empty(true)
			{ };

			CLVException(const char* msg)
				: exception(msg), m_Id(""), m_Error(0), m_Line(-1), m_File(""), m_Fnc(""), m_Empty(false)
			{  };

			CLVException(const char* id, const char* msg)
				: exception(msg), m_Id(id), m_Error(0), m_Line(-1), m_File(""), m_Fnc(""), m_Empty(false) 
			{  };

			CLVException(const char* id, const char* msg, const char* file, int line)
				: exception(msg), m_Id(id), m_Error(0), m_Line(line), m_File(file), m_Fnc(""), m_Empty(false)
			{  };

			CLVException(const char* id, const char* msg, const char* file, int line, const char* fnc)
				: exception(msg), m_Id(id), m_Error(0), m_Line(line), m_File(file), m_Fnc(fnc), m_Empty(false)
			{  };

			CLVException(int error, const char* msg)
				: exception(msg), m_Error(error), m_Line(-1), m_File(""), m_Empty(false)  
			{ };

			CLVException(int error, const char* msg, const char* file, int line)
				: exception(msg), m_Error(error), m_Line(line), m_File(file), m_Empty(false)
			{ };

			CLVException(CLVException* e)
				: exception(e->what()), m_Id(e->id()), m_Error(e->error()), m_Line(e->line()), m_File(e->file()), m_Fnc(e->fnc()), m_Empty(false)
			{ };

			int error() { return m_Error; };
			const char* id() { return m_Id.c_str(); }
			const char* file() { return m_File.c_str(); }
			const char* fnc() { return m_Fnc.c_str(); }
			int line() { return m_Line; }
			bool empty() { return m_Empty; }

			//exception format function simplified
			void str(char* msg, size_t msgSize) { sprintf_s(msg, msgSize, "[%s] %s;", id(), what()); };
			//exception format function extendet
			void str_ext(char* msg, size_t msgSize) { sprintf_s(msg, msgSize, "[%s] %s; function: %s; file: %s; line: %d;", id(), what(), fnc(), file(), line()); };

		public:
			typedef shared_ptr<CLVException> CLVExceptionPtr;

		private:
			bool m_Empty;
			int m_Error;
			int m_Line;
			string m_File;
			string m_Id;
			string m_Fnc;
			CLVException* m_E;
		};
	}
}

