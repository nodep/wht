#include "stdafx.h"

#include "hid.h"
#include "dllwrap.h"

Dll hidDll;

void (__stdcall *HidD_GetHidGuid) (LPGUID HidGuid) = 0;
BOOLEAN (__stdcall *HidD_GetAttributes) (HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes) = 0;
BOOLEAN (__stdcall *HidD_GetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;
BOOLEAN (__stdcall *HidD_SetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;
BOOLEAN (__stdcall *HidD_GetInputReport) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength) = 0;
BOOLEAN (__stdcall *HidD_GetProductString) (HANDLE HidDeviceObject, PVOID Buffer, ULONG BufferLength) = 0;

NTSTATUS (__stdcall *HidP_GetCaps) (PHIDP_PREPARSED_DATA PreparsedData, PHIDP_CAPS Capabilities) = 0;
NTSTATUS (__stdcall *HidP_GetButtonCaps) (HIDP_REPORT_TYPE ReportType, PHIDP_BUTTON_CAPS ButtonCaps, PUSHORT ButtonCapsLength, PHIDP_PREPARSED_DATA PreparsedData) = 0;
NTSTATUS (__stdcall *HidP_GetValueCaps) (HIDP_REPORT_TYPE ReportType, PHIDP_VALUE_CAPS ValueCaps, PUSHORT ValueCapsLength, PHIDP_PREPARSED_DATA PreparsedData) = 0;
NTSTATUS (__stdcall *HidP_GetUsages) (HIDP_REPORT_TYPE ReportType, USAGE UsagePage, USHORT LinkCollection, PUSAGE UsageList, PULONG UsageLength, PHIDP_PREPARSED_DATA PreparsedData, PCHAR Report, ULONG ReportLength) = 0;
NTSTATUS (__stdcall *HidP_GetUsageValue) (HIDP_REPORT_TYPE ReportType, USAGE UsagePage, USHORT LinkCollection, USAGE Usage, PULONG UsageValue, PHIDP_PREPARSED_DATA PreparsedData, PCHAR Report, ULONG ReportLength) = 0;

#define CHECK_FUNC(func)		if (!func) msg += L"\n" L ## #func

bool InitHID()
{
	if (!hidDll.Load(L"hid.dll"))
	{
		MessageBox(0, L"Unable to load hid.dll", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// get the function from the loaded DLLs
	hidDll.GetProc(HidD_GetHidGuid,		"HidD_GetHidGuid");
	hidDll.GetProc(HidD_GetAttributes,	"HidD_GetAttributes");
	hidDll.GetProc(HidD_GetFeature,		"HidD_GetFeature");
	hidDll.GetProc(HidD_SetFeature,		"HidD_SetFeature");
	hidDll.GetProc(HidD_GetInputReport,	"HidD_GetInputReport");
	hidDll.GetProc(HidD_GetProductString, "HidD_GetProductString");

	hidDll.GetProc(HidP_GetCaps,		"HidP_GetCaps");
	hidDll.GetProc(HidP_GetButtonCaps,	"HidP_GetButtonCaps");
	hidDll.GetProc(HidP_GetValueCaps,	"HidP_GetValueCaps");
	hidDll.GetProc(HidP_GetUsages,		"HidP_GetUsages");
	hidDll.GetProc(HidP_GetUsageValue,	"HidP_GetUsageValue");

	if (!HidD_GetHidGuid   ||  !HidD_GetAttributes  ||  !HidD_GetFeature  ||  !HidD_SetFeature  ||  !HidD_GetInputReport
			|| !HidP_GetCaps  ||  !HidP_GetButtonCaps  ||  !HidP_GetValueCaps  ||  !HidP_GetUsages ||  !HidP_GetUsageValue)
	{
		std::wstring msg(L"Unable to load all the necessary functions from hid.dll\n\nMissing functions:\n");

		CHECK_FUNC(HidD_GetHidGuid);
		CHECK_FUNC(HidD_GetAttributes);
		CHECK_FUNC(HidD_GetFeature);
		CHECK_FUNC(HidD_SetFeature);
		CHECK_FUNC(HidD_GetInputReport);
		CHECK_FUNC(HidD_GetProductString);

		CHECK_FUNC(HidP_GetCaps);
		CHECK_FUNC(HidP_GetButtonCaps);
		CHECK_FUNC(HidP_GetValueCaps);
		CHECK_FUNC(HidP_GetUsages);
		CHECK_FUNC(HidP_GetUsageValue);

		MessageBox(0, msg.c_str(), L"Fatal error", MB_OK | MB_ICONERROR);

		return false;
	}

	return true;
}