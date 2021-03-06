#include "stdafx.h"

#include "hid.h"
#include "my_utils.h"

#include "../dongle/reports.h"
#include "wht_dongle.h"

#define VENDOR_ID	0x40AA
#define PRODUCT_ID	0x9005

WHTDongle::WHTDongle()
	: hDevice(NULL)
{}

WHTDongle::~WHTDongle()
{
	Close();
}

bool WHTDongle::Open()
{
	Close();

	hDevice = NULL;

	GUID guid;
	HDEVINFO info;
	DWORD index, required_size;
	std::vector<char> buff;

	HidD_GetHidGuid(&guid);
	info = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (info == INVALID_HANDLE_VALUE)
		return false;

	for (index = 0; 1; index++)
	{
		// get the next HID device
		SP_DEVICE_INTERFACE_DATA iface;
		iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		if (!SetupDiEnumDeviceInterfaces(info, NULL, &guid, index, &iface))
		{
			SetupDiDestroyDeviceInfoList(info);
			break;
		}

		// get the required size for details 
		SetupDiGetDeviceInterfaceDetail(info, &iface, NULL, 0, &required_size, NULL);

		// allocate and clear
		buff.clear();
		buff.insert(buff.end(), required_size, '\0');

		SP_DEVICE_INTERFACE_DETAIL_DATA* details = (SP_DEVICE_INTERFACE_DETAIL_DATA*) &buff.front();

		// now get the details
		details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (!SetupDiGetDeviceInterfaceDetail(info, &iface, details, required_size, NULL, NULL))
			continue;

		// open the HID device
		HANDLE h = CreateFile(details->DevicePath, GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
						FILE_FLAG_OVERLAPPED, NULL);

		if (h == INVALID_HANDLE_VALUE)
			continue;

		// get the device's attributes
		HIDD_ATTRIBUTES attrib;
		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		if (!HidD_GetAttributes(h, &attrib))
		{
			CloseHandle(h);
			continue;
		}
		
		// is this the device we need?
		if (attrib.VendorID != VENDOR_ID  ||  attrib.ProductID != PRODUCT_ID)
		{
			CloseHandle(h);
			continue;
		}

		SetupDiDestroyDeviceInfoList(info);
		hDevice = h;
	}

	if (!hDevice)
		return false;

	return true;
}

void WHTDongle::Close()
{
	if (!hDevice)
		return;

	CloseHandle(hDevice);

	hDevice = NULL;
}

void WHTDongle::ThrowException(const wchar_t* during, int report_id)
{
	int last_error = GetLastError();
	std::wstring msg(during);
	msg += L" failed!\nPlease check if the dongle is connected.\nReport ID = " + int2str(report_id) + L"\nGetLastError() = " + int2str(last_error);
	throw msg;
}

void WHTDongle::GetFeatureReportRaw(void* buffer, int report_size)
{
	if (HidD_GetFeature(hDevice, buffer, report_size) != TRUE)
		ThrowException(L"HidD_GetFeature", ((uint8_t*) buffer)[0]);
}

void WHTDongle::SetFeatureReportRaw(const void* buffer, int report_size)
{
	if (HidD_SetFeature(hDevice, (PVOID) buffer, report_size) != TRUE)
		ThrowException(L"HidD_SetFeature", ((uint8_t*) buffer)[0]);
}

void WHTDongle::GetInputReportRaw(void* buffer, int report_size)
{
	if (HidD_GetInputReport(hDevice, buffer, report_size) != TRUE)
		ThrowException(L"HidD_GetInputReport", ((uint8_t*) buffer)[0]);
}

bool WHTDongle::ReadProtocolVersion(protocol_version_e& dongle_ver, protocol_version_e& tracker_ver)
{
	FeatRep_ProtocolVersion protVer;
	protVer.report_id = PROT_VER_REPORT_ID;
	if (HidD_GetFeature(hDevice, &protVer, sizeof(protVer)) != TRUE)
		return false;

	dongle_ver = (protocol_version_e) protVer.dongle_ver;
	tracker_ver = (protocol_version_e) protVer.tracker_ver;

	return true;
}