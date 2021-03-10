#include "CUsbHidDevice.h"

CUsbHidDevice::CUsbHidDevice(const CUsbHidDevice&)
{

}

CUsbHidDevice& CUsbHidDevice::operator=(const CUsbHidDevice&)
{
	return *instance;
}

int CUsbHidDevice::CommunicateHid(unsigned char* buf)
{
	if (m_pHandle == NULL)
	{
		if (OpenDevice() <= 0)
		{
			return -1;
		}
	}

	int res = hid_write(m_pHandle, buf, 65);
	if (res < 0) 
	{
		CloseDevice();
		return -1;
	}
	else
	{
		return 1;
	}
}

CUsbHidDevice* CUsbHidDevice::getInstance()
{

	return instance;
}

DWORD WINAPI CUsbHidDevice::HidRespThreadFunc(LPVOID lpParamter)
{
	unsigned char buf[256];
	while (true)
	{
		if (Polling(buf) > 0)
		{
			RESP_INTERFACE_LIST::iterator iterator;

			for (iterator = m_listHidRespInterface.begin(); iterator != m_listHidRespInterface.end(); ++iterator)
			{
				CHidRespInterface* pInterface = (CHidRespInterface*)*iterator;

				if (buf[1] == 0xc8)
				{
					pInterface->OnCommandResp(buf);
				}
				else
				{
					pInterface->OnSensorEvent(buf);
				}
			}
		}
		else
		{
			return -1;
		}
	}
	
	return 0;
}


int CUsbHidDevice::OpenDevice()
{
	if (NULL != m_pHandle)
	{
		CloseDevice();
	}

	if (hid_init())
	{
		return -1;
	}

	struct hid_device_info *devs, *cur_dev;
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		if ((0x483 == cur_dev->vendor_id) && (0x5750 == cur_dev->product_id))
		{
			m_pHandle = hid_open(0x483, 0x5750, NULL);
			break;
		}
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
	
	if (NULL == m_pHandle) {
		return -1;
	}

	hid_set_nonblocking(m_pHandle, 1);

	m_hidRespThread = CreateThread(NULL, 0, HidRespThreadFunc, NULL, 0, NULL);

	return 1;
}

void CUsbHidDevice::CloseDevice()
{
	if (NULL == m_pHandle)
	{
		return;
	}
	else
	{
		if (m_hidRespThread != 0)
		{
			CloseHandle(m_hidRespThread);
		}

		hid_close(m_pHandle);
		m_pHandle = NULL;
		hid_exit();
	}
}

int CUsbHidDevice::GetVersion()
{
	unsigned char buf[256];
	memset(buf, 0, 256);

	buf[0] = 0x0;
	buf[1] = 0x66;
	buf[2] = 0x00;
	
	return CommunicateHid(buf);
}

int CUsbHidDevice::SetScreenResolution(unsigned char resolution)
{
	unsigned char buf[256];
	memset(buf, 0, 256);

	buf[0] = 0x1;
	buf[1] = 0x66;
	buf[2] = resolution;
	return  CommunicateHid(buf);
}

int CUsbHidDevice::Polling(unsigned char *buf)
{
	if (m_pHandle == NULL)
	{
		return -1;
	}

	return hid_read_timeout(m_pHandle, buf, 64, -1);
}

int CUsbHidDevice::SetScreenResolution1920x1080()
{
	return SetScreenResolution(0x07);
}

int CUsbHidDevice::SetScreenResolution3840x1080()
{
	return SetScreenResolution(0x06);
}

int CUsbHidDevice::StartSensor()
{
	unsigned char buf[256];
	memset(buf, 0, 256);

	buf[0] = 0x1;
	buf[1] = 0x66;
	buf[2] = 0x01;

	return CommunicateHid(buf);
}

int CUsbHidDevice::StopSensor()
{
	unsigned char buf[256];
	memset(buf, 0, 256);

	buf[0] = 0x1;
	buf[1] = 0x66;
	buf[2] = 0x02;

	return CommunicateHid(buf);
}

void CUsbHidDevice::RegisterRespCallback(CHidRespInterface * pInterface)
{
	RESP_INTERFACE_LIST::iterator iterator;

	iterator = find(m_listHidRespInterface.begin(), m_listHidRespInterface.end(), pInterface);

	if (iterator == m_listHidRespInterface.end())
	{
		m_listHidRespInterface.push_back(pInterface);
	}
}

void CUsbHidDevice::UnregisterRespCallback(CHidRespInterface * pInterface)
{
	RESP_INTERFACE_LIST::iterator iterator;

	iterator = find(m_listHidRespInterface.begin(), m_listHidRespInterface.end(), pInterface);

	if (iterator != m_listHidRespInterface.end())
	{
		m_listHidRespInterface.erase(iterator);
	}
}


CUsbHidDevice* CUsbHidDevice::instance = new CUsbHidDevice();
hid_device* CUsbHidDevice::m_pHandle = NULL;
RESP_INTERFACE_LIST CUsbHidDevice::m_listHidRespInterface;