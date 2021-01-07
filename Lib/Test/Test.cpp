// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//#include "pch.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <iostream>

#define TEST_MODBUSC

#ifdef TEST_CLV
#include "..\CLVLib\CLVProtocol.h"


using namespace Utilities;

class CLVTest
{
public:
	void static OnValueChanged(CLVProtocol* sender, const char *barcode, UINT64 barcodeLen)
	{
		printf("event callback: %s", barcode);
	}
};

void CLVTest()
{
	try
	{
		CLVProtocol *clv = new CLVProtocol("COM1", Serial::BR_9600);

		clv->OnValueChanged += &CLVTest::OnValueChanged;

		clv->SelfTest();
		clv->ReadingMode();
		clv->StartReading(true, 5000);

		do
		{
			Sleep(5);
		} while (true);

	}
	catch (CLVException& e)
	{
		printf("%s", e.what());
		printf("\r\n");
	}
}

#endif

#ifdef TEST_FVEDIO
//#include "..\FVEDIOLib\FVEDIOProtocol.h"
//#include "..\FVEDIOLib\FVEDIOException.h"

using namespace Utilities::FVEDIO;

void FVEDIOTest()
{
	try
	{
		FVEDIOProtocol  *fvedio = new FVEDIOProtocol("COM1", Serial::BR_57600);
		fvedio->WriteDO(0xffff);
		fvedio->LED0(0);
		fvedio->LED1(0);
		fvedio->LED2(0);
		fvedio->LED3(0);

		fvedio->WriteSingleDO(4, 1);
		fvedio->WriteSingleDO(12, 1);

		fvedio->WriteSingleDO(4, 0);
		fvedio->WriteSingleDO(12, 0);

		fvedio->WriteDO(0xffff);

	}
	catch (FVEDIOException& e)
	{
		printf("%s", e.what());
		printf("\r\n");
	}
}
#endif

#ifdef TEST_MODBUS
//#include "..\ModbusLib\ModBusLib.h"
#include "..\ModBusWrapperLib\ModBusWrapper.h"

using namespace Utilities;

void ModBusTest(void)
{
	try
	{
#if 0
		ModBusWrapperLib *modbus = new ModBusWrapperLib();
		modbus->Initialize();
		modbus->Start();

		UINT16 wval = 0;
		modbus->Read("HEAD.NAV", &wval);
		modbus->Read("TESTER.IAI_POS_ACT", &wval);

		modbus->Stop();
#endif
		//Singelton access
		ModBusWrapperLib::Instance()->Initialize();
		ModBusWrapperLib::Instance()->Start();

		UINT16 wval = 0;
		ModBusWrapperLib::Instance()->Read("PLC.TESTER.IsMaskFront", &wval);
		ModBusWrapperLib::Instance()->Read("PLC.AxeXPositionAct", &wval);

		ModBusWrapperLib::Instance()->Stop();
	}
	catch (ModBusWrapperException& e)
	{
		char errMsg[1024];
		e.str_ext(errMsg, sizeof(errMsg) - 1);
		printf(errMsg);

		//printf(e.what());
		printf("\r\n");
	}
}
#endif

#ifdef TEST_MODBUSC

#include "..\ModBusWrapperLib\ModBusWrapperC.h"

void OnValueChanged(const char*, UINT64)
{

}

void ModBusTest(void)
{
	int error = 0;
	UINT16 wval = 0;

	SModBusPtr modbus = NULL;
	modbus_new(&modbus);

	error = modbus->Initialize(modbus);
	if (error < 0)
	{
		char msg[512];
		ModBusGetLastError(msg, sizeof(msg) - 1);
		goto Error;
	}
	error = modbus->Start(modbus);
	if (error < 0)
	{
		char msg[512];
		ModBusGetLastError(msg, sizeof(msg) - 1);
		goto Error;
	}

	error = modbus->OnValueChanged(modbus, "PLC.TESTER.IsMaskFront", OnValueChanged);
	if (error < 0)
	{
		char msg[512];
		ModBusGetLastError(msg, sizeof(msg) - 1);
		goto Error;
	}

	error = modbus->Read(modbus, "PLC.TESTER.IsMaskFront", &wval);
	if (error < 0)
	{
		char msg[512];
		ModBusGetLastError(msg, sizeof(msg) - 1);
		goto Error;
	}
	error = modbus->Read(modbus, "PLC.AxeXPositionAct", &wval);
	if (error < 0)
	{
		char msg[512];
		ModBusGetLastError(msg, sizeof(msg) - 1);
		goto Error;
	}

	error = modbus->Stop(modbus);
	if (error < 0)
	{
		char msg[512];
		ModBusGetLastError(msg, sizeof(msg) - 1);
		return;
	}

Error:
	modbus_delete(&modbus);
}
#endif

int main()
{
	//dependency lib1
	HMODULE hLibrary1 = LoadLibrary(L"C:\\Work\\Development\\Lib\\Build\\debug\\dll\\SharedNodes.dll");
	//dependency lib2
	HMODULE hLibrary2 = LoadLibrary(L"C:\\Work\\Development\\Lib\\Build\\debug\\dll\\Modbus.dll");
	//Linker /DELAYLOAD:ModBusWrapper.dll
	HMODULE hLibrary3 = LoadLibrary(L"C:\\Work\\Development\\Lib\\Build\\debug\\dll\\ModBusWrapper.dll");
	if (hLibrary3 == 0)
	{
		printf("LoadLibrary failed\n");
		return -1;
	}

	ModBusTest();
}