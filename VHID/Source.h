#pragma once
#include <ntddk.h>
#include <wdf.h>
#include <vhf.h>
#include <initguid.h>

#define TraceEnterFunc() KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "VHID: Enter %s, IRQL: %d\n", __FUNCTION__, KeGetCurrentIrql()))
#define TraceErrorStatus(funcName, status) KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "VHID: %s returned 0x%x in %s\n", funcName, status, __FUNCTION__))

#if DBG

#define TraceInfo(msg, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "VHID: " msg "\n", __VA_ARGS__)
#define TraceError(msg, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "VHID: " msg "\n", __VA_ARGS__)

#else

#define TraceInfo(msg, ...)
#define TraceError(msg, ...)

#endif

#define MAX_ID_LEN 128

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD virtual_hid_driver_evt_device_add;
EVT_VHF_CLEANUP vhf_source_device_cleanup;
EVT_VHF_CLEANUP vhf_cleanup;
EVT_WDF_IO_QUEUE_IO_WRITE evt_io_write_for_report_queue;
EVT_WDF_IO_QUEUE_IO_WRITE evt_io_write_for_forward_queue;
_IRQL_requires_(PASSIVE_LEVEL);

typedef struct _DEVICE_CONTEXT
{
    VHFHANDLE vhf_handle;
	WDFQUEUE rep_queue;
	WDFDEVICE forward_device;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

typedef struct _REPORT_QUEUE_CONTEXT
{
	WDFQUEUE queue;
	PDEVICE_CONTEXT device_context;
} REPORT_QUEUE_CONTEXT, *PREPORT_QUEUE_CONTEXT;

typedef struct _FORWARD_DEVICE_CONTEXT{
	ULONG instance_no;
	WDFQUEUE parent_queue;
} FORWARD_DEVICE_CONTEXT, *PFORWARD_DEVICE_CONTEXT;

DEFINE_GUID(GUID_DEVINTERFACE_VHID, 0x1b2d3f62, 0x0326, 0x4889,
	0xa5, 0xc2, 0x48, 0xae, 0x3e, 0x99, 0x53, 0x70);

DEFINE_GUID(GUID_DEVCLASS_VHID, 0x908f6b59, 0xd702, 0x4b45,
	0xad, 0x3c, 0x9f, 0xa0, 0xbf, 0xc5, 0x84, 0xab);

DEFINE_GUID(GUID_BUS_VHID, 0x6141db2e, 0x6dbd, 0x401b,
	0xb2, 0xaf, 0x9d, 0x81, 0x73, 0xc1, 0x83, 0x63);

#define VHID_DEVICE_ID L"{6141DB2E-6DBD-401B-B2AF-9D8173C18363}\\VHIDBus\0"


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REPORT_QUEUE_CONTEXT, GetReportQueueContext)
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FORWARD_DEVICE_CONTEXT, GetForwardDeviceContext)

NTSTATUS raw_queue_create(_In_ WDFDEVICE, _Out_ WDFQUEUE*);
NTSTATUS forward_device_create(WDFDEVICE);
NTSTATUS vhf_initialize(_In_ WDFDEVICE);