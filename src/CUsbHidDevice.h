#pragma once

#include <Windows.h>
#include <iostream>
#include <list>

#include "hidapi.h"
#include "CHidRespInterface.h"

using namespace std;

typedef list<CHidRespInterface*> RESP_INTERFACE_LIST;

class CUsbHidDevice
{
private:
	CUsbHidDevice() 
	{
		m_pHandle = NULL;
		m_rotation = 0;
	}
	~CUsbHidDevice() 
	{
		CloseDevice();
	}

	CUsbHidDevice(const CUsbHidDevice&);
	CUsbHidDevice& operator=(const CUsbHidDevice&);
	static CUsbHidDevice* instance;

private:
	static hid_device *m_pHandle;
	unsigned char m_rotation = 0;
	HANDLE m_hidRespThread = 0;
	static RESP_INTERFACE_LIST m_listHidRespInterface;

	int CommunicateHid(unsigned char* buf);
	int SetScreenResolution(unsigned char resolution);
	static DWORD WINAPI HidRespThreadFunc(LPVOID lpParamter);
	static int Polling(unsigned char* buf);

public:
	static CUsbHidDevice* getInstance();

	int OpenDevice();
	void CloseDevice();

	int GetVersion();

	/* 120 - 0x01, 300 - 0x02, 500 - 0x00, 1500 - 0x03, 3000 - 0x04 */
	int SetScreenResolution1920x1080();
	int SetScreenResolution3840x1080();

	int StartSensor();
	int StopSensor();

	static void RegisterRespCallback(CHidRespInterface* pInterface);
	static void UnregisterRespCallback(CHidRespInterface* pInterface);

};