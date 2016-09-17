#pragma once

// We define only the stuff we need from the DDK and load it dynamically from the DLLs.
// This is dirty, of course, but it beats installing the entire Windows DDK because
// of a few functions and structures

#include <setupapi.h>

typedef struct _HIDP_PREPARSED_DATA * PHIDP_PREPARSED_DATA;
typedef USHORT USAGE, *PUSAGE;

typedef enum _HIDP_REPORT_TYPE
{
    HidP_Input,
    HidP_Output,
    HidP_Feature
} HIDP_REPORT_TYPE;

typedef struct _HIDD_ATTRIBUTES {
    ULONG   Size; // = sizeof (struct _HIDD_ATTRIBUTES)

    //
    // Vendor ids of this hid device
    //
    USHORT  VendorID;
    USHORT  ProductID;
    USHORT  VersionNumber;

    //
    // Additional fields will be added to the end of this structure.
    //
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

typedef struct _HIDP_CAPS
{
    USAGE    Usage;
    USAGE    UsagePage;
    USHORT   InputReportByteLength;
    USHORT   OutputReportByteLength;
    USHORT   FeatureReportByteLength;
    USHORT   Reserved[17];

    USHORT   NumberLinkCollectionNodes;

    USHORT   NumberInputButtonCaps;
    USHORT   NumberInputValueCaps;
    USHORT   NumberInputDataIndices;

    USHORT   NumberOutputButtonCaps;
    USHORT   NumberOutputValueCaps;
    USHORT   NumberOutputDataIndices;

    USHORT   NumberFeatureButtonCaps;
    USHORT   NumberFeatureValueCaps;
    USHORT   NumberFeatureDataIndices;
} HIDP_CAPS, *PHIDP_CAPS;

typedef struct _HIDP_BUTTON_CAPS
{
    USAGE    UsagePage;
    UCHAR    ReportID;
    BOOLEAN  IsAlias;

    USHORT   BitField;
    USHORT   LinkCollection;   // A unique internal index pointer

    USAGE    LinkUsage;
    USAGE    LinkUsagePage;

    BOOLEAN  IsRange;
    BOOLEAN  IsStringRange;
    BOOLEAN  IsDesignatorRange;
    BOOLEAN  IsAbsolute;

    ULONG    Reserved[10];
    union {
        struct {
            USAGE    UsageMin,         UsageMax;
            USHORT   StringMin,        StringMax;
            USHORT   DesignatorMin,    DesignatorMax;
            USHORT   DataIndexMin,     DataIndexMax;
        } Range;
        struct  {
            USAGE    Usage,            Reserved1;
            USHORT   StringIndex,      Reserved2;
            USHORT   DesignatorIndex,  Reserved3;
            USHORT   DataIndex,        Reserved4;
        } NotRange;
    };

} HIDP_BUTTON_CAPS, *PHIDP_BUTTON_CAPS;

typedef struct _HIDP_VALUE_CAPS
{
    USAGE    UsagePage;
    UCHAR    ReportID;
    BOOLEAN  IsAlias;

    USHORT   BitField;
    USHORT   LinkCollection;   // A unique internal index pointer

    USAGE    LinkUsage;
    USAGE    LinkUsagePage;

    BOOLEAN  IsRange;
    BOOLEAN  IsStringRange;
    BOOLEAN  IsDesignatorRange;
    BOOLEAN  IsAbsolute;

    BOOLEAN  HasNull;        // Does this channel have a null report   union
    UCHAR    Reserved;
    USHORT   BitSize;        // How many bits are devoted to this value?

    USHORT   ReportCount;    // See Note below.  Usually set to 1.
    USHORT   Reserved2[5];

    ULONG    UnitsExp;
    ULONG    Units;

    LONG     LogicalMin,       LogicalMax;
    LONG     PhysicalMin,      PhysicalMax;

    union {
        struct {
            USAGE    UsageMin,         UsageMax;
            USHORT   StringMin,        StringMax;
            USHORT   DesignatorMin,    DesignatorMax;
            USHORT   DataIndexMin,     DataIndexMax;
        } Range;

        struct {
            USAGE    Usage,            Reserved1;
            USHORT   StringIndex,      Reserved2;
            USHORT   DesignatorIndex,  Reserved3;
            USHORT   DataIndex,        Reserved4;
        } NotRange;
    };
} HIDP_VALUE_CAPS, *PHIDP_VALUE_CAPS;

extern void (__stdcall *HidD_GetHidGuid) (LPGUID HidGuid);
extern BOOLEAN (__stdcall *HidD_GetAttributes) (HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes);
extern BOOLEAN (__stdcall *HidD_GetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);
extern BOOLEAN (__stdcall *HidD_SetFeature) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);
extern BOOLEAN (__stdcall *HidD_GetInputReport) (HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);
extern BOOLEAN (__stdcall *HidD_GetProductString) (HANDLE HidDeviceObject, PVOID Buffer, ULONG BufferLength);

extern NTSTATUS (__stdcall *HidP_GetCaps) (PHIDP_PREPARSED_DATA PreparsedData, PHIDP_CAPS Capabilities);
extern NTSTATUS (__stdcall *HidP_GetButtonCaps) (HIDP_REPORT_TYPE ReportType, PHIDP_BUTTON_CAPS ButtonCaps, PUSHORT ButtonCapsLength, PHIDP_PREPARSED_DATA PreparsedData);
extern NTSTATUS (__stdcall *HidP_GetValueCaps) (HIDP_REPORT_TYPE ReportType, PHIDP_VALUE_CAPS ValueCaps, PUSHORT ValueCapsLength, PHIDP_PREPARSED_DATA PreparsedData);
extern NTSTATUS (__stdcall *HidP_GetUsages) (HIDP_REPORT_TYPE ReportType, USAGE UsagePage, USHORT LinkCollection, PUSAGE UsageList, PULONG UsageLength, PHIDP_PREPARSED_DATA PreparsedData, PCHAR Report, ULONG ReportLength);
extern NTSTATUS (__stdcall *HidP_GetUsageValue) (HIDP_REPORT_TYPE ReportType, USAGE UsagePage, USHORT LinkCollection, USAGE Usage, PULONG UsageValue, PHIDP_PREPARSED_DATA PreparsedData, PCHAR Report, ULONG ReportLength);

#ifndef FACILITY_HID_ERROR_CODE
#define FACILITY_HID_ERROR_CODE 0x11
#endif

#define HIDP_ERROR_CODES(SEV, CODE)          ((NTSTATUS) (((SEV) << 28) | (FACILITY_HID_ERROR_CODE << 16) | (CODE)))

#define HIDP_STATUS_SUCCESS                  (HIDP_ERROR_CODES(0x0,0))
#define HIDP_STATUS_NULL                     (HIDP_ERROR_CODES(0x8,1))
#define HIDP_STATUS_INVALID_PREPARSED_DATA   (HIDP_ERROR_CODES(0xC,1))
#define HIDP_STATUS_INVALID_REPORT_TYPE      (HIDP_ERROR_CODES(0xC,2))
#define HIDP_STATUS_INVALID_REPORT_LENGTH    (HIDP_ERROR_CODES(0xC,3))
#define HIDP_STATUS_USAGE_NOT_FOUND          (HIDP_ERROR_CODES(0xC,4))
#define HIDP_STATUS_VALUE_OUT_OF_RANGE       (HIDP_ERROR_CODES(0xC,5))
#define HIDP_STATUS_BAD_LOG_PHY_VALUES       (HIDP_ERROR_CODES(0xC,6))
#define HIDP_STATUS_BUFFER_TOO_SMALL         (HIDP_ERROR_CODES(0xC,7))
#define HIDP_STATUS_INTERNAL_ERROR           (HIDP_ERROR_CODES(0xC,8))
#define HIDP_STATUS_I8042_TRANS_UNKNOWN      (HIDP_ERROR_CODES(0xC,9))
#define HIDP_STATUS_INCOMPATIBLE_REPORT_ID   (HIDP_ERROR_CODES(0xC,0xA))
#define HIDP_STATUS_NOT_VALUE_ARRAY          (HIDP_ERROR_CODES(0xC,0xB))
#define HIDP_STATUS_IS_VALUE_ARRAY           (HIDP_ERROR_CODES(0xC,0xC))
#define HIDP_STATUS_DATA_INDEX_NOT_FOUND     (HIDP_ERROR_CODES(0xC,0xD))
#define HIDP_STATUS_DATA_INDEX_OUT_OF_RANGE  (HIDP_ERROR_CODES(0xC,0xE))
#define HIDP_STATUS_BUTTON_NOT_PRESSED       (HIDP_ERROR_CODES(0xC,0xF))
#define HIDP_STATUS_REPORT_DOES_NOT_EXIST    (HIDP_ERROR_CODES(0xC,0x10))
#define HIDP_STATUS_NOT_IMPLEMENTED          (HIDP_ERROR_CODES(0xC,0x20))

bool InitHID();
