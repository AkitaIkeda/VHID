#include "Source.h"
#include "RepDesc.h"
#include "ntstrsafe.h"
#include "VHIDUtill.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, virtual_hid_driver_evt_device_add)
#pragma alloc_text(PAGE, vhf_source_device_cleanup)
#pragma alloc_text(PAGE, evt_io_write_for_report_queue)
#endif

ULONG instance_no = 0;

NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT     driver_object,
    _In_ PUNICODE_STRING    registry_path
)
{
    TraceEnterFunc();
	
    // NTSTATUS variable to record success or failure
    NTSTATUS status = STATUS_SUCCESS;

    // Allocate the driver configuration object
    WDF_DRIVER_CONFIG config;

    // Initialize the driver configuration object to register the
    // entry point for the EvtDeviceAdd callback, KmdfHelloWorldEvtDeviceAdd
    WDF_DRIVER_CONFIG_INIT(&config,
        virtual_hid_driver_evt_device_add
    );

    // Finally, create the driver object
    status = WdfDriverCreate(driver_object,
        registry_path,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );
    return status;
}

NTSTATUS virtual_hid_driver_evt_device_add(
    _In_    WDFDRIVER       driver,
    _Inout_ PWDFDEVICE_INIT device_init
)
{
	// We're not using the driver object,
    // so we need to mark it as unreferenced
    UNREFERENCED_PARAMETER(driver);
    PAGED_CODE();
    TraceEnterFunc();

	WDF_OBJECT_ATTRIBUTES   device_attributes;
    WDFDEVICE device;
    NTSTATUS status;
    PDEVICE_CONTEXT deviceContext;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&device_attributes, DEVICE_CONTEXT);
    device_attributes.EvtCleanupCallback = vhf_source_device_cleanup;
	
    // Create the device object
    status = WdfDeviceCreate(&device_init,
        &device_attributes, &device);
    if (!NT_SUCCESS(status)) {
        TraceErrorStatus("WdfDeviceCreate", status);
        return status;
    }

    deviceContext = GetDeviceContext(device);
	
    vhf_initialize(device);
	
	raw_queue_create(device, &deviceContext->rep_queue);

    forward_device_create(device);

    return status;
}

VOID vhf_source_device_cleanup(
    _In_ WDFOBJECT device_object
)
{
	PAGED_CODE();
    TraceEnterFunc();
    PDEVICE_CONTEXT device_context = GetDeviceContext(device_object);
	if (device_context->vhf_handle != WDF_NO_HANDLE)
        VhfDelete(device_context->vhf_handle, TRUE);
}

NTSTATUS raw_queue_create(_In_ WDFDEVICE device, _Out_ WDFQUEUE* queue)
{
    NTSTATUS                status;
    WDF_IO_QUEUE_CONFIG     queue_config;
    WDF_OBJECT_ATTRIBUTES   queue_attributes;
    PREPORT_QUEUE_CONTEXT   queue_context;

    WDF_IO_QUEUE_CONFIG_INIT(&queue_config, WdfIoQueueDispatchSequential);
    queue_config.EvtIoWrite = evt_io_write_for_report_queue;
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queue_attributes, REPORT_QUEUE_CONTEXT);

    status = WdfIoQueueCreate(device, &queue_config, &queue_attributes, queue);
    if (!NT_SUCCESS(status)) {
        TraceErrorStatus("WdfIoQueueCreate", status);
        return status;
    }
    queue_context = GetReportQueueContext(*queue);
	queue_context->device_context = GetDeviceContext(device);
    queue_context->queue = *queue;
	
    return status;
}

VOID evt_io_write_for_report_queue(
    _In_ WDFQUEUE   queue,
    _In_ WDFREQUEST req,
    _In_ SIZE_T     len
)
{
    TraceEnterFunc();
    PREPORT_QUEUE_CONTEXT rep_queuec_context;
    WDFMEMORY mem;
    VOID* pvoid;
    SIZE_T length;
    HID_XFER_PACKET trans_packet;
    NTSTATUS status;

    rep_queuec_context = GetReportQueueContext(queue);

    if (len != sizeof(HID_INPUT_REPORT))
    {
        KdPrint(("VHID: invalid input buffer. size %d, expect %d\n",
            len, sizeof(HID_INPUT_REPORT)));
        WdfRequestComplete(req, STATUS_INVALID_BUFFER_SIZE);
        return;
    }

    status = WdfRequestRetrieveInputMemory(req, &mem);
    if (!NT_SUCCESS(status)) {
        WdfRequestComplete(req, status);
    	return;
    }

    pvoid = WdfMemoryGetBuffer(mem, &length);
	if(pvoid == NULL)
	{
        TraceErrorStatus("WdfMemoryGetBuffer", status);
        WdfRequestComplete(req, STATUS_INVALID_BUFFER_SIZE);
		return;
	}

    trans_packet.reportId = *(PUCHAR)pvoid;
    trans_packet.reportBuffer = (PUCHAR)pvoid;
    trans_packet.reportBufferLen = (ULONG)len;
    for (SIZE_T i = 0; i < len; ++i)
        KdPrint(("VHID: rep[%d] = %d", i, ((PUCHAR)pvoid)[i]));
    status = VhfReadReportSubmit(rep_queuec_context->device_context->vhf_handle, &trans_packet);
    if (!NT_SUCCESS(status)) TraceErrorStatus("VhfReadReportSubmit", status);
    WdfRequestCompleteWithInformation(req, status, len);
}

NTSTATUS forward_device_create(WDFDEVICE device)
{
    NTSTATUS status = STATUS_SUCCESS;
    PWDFDEVICE_INIT p_device_init = NULL;
    PFORWARD_DEVICE_CONTEXT context = NULL;
    WDFDEVICE child = NULL;
    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_IO_QUEUE_CONFIG queue_config;
    WDFQUEUE queue;
    WDF_DEVICE_STATE device_state;
    PDEVICE_CONTEXT device_context;

    DECLARE_CONST_UNICODE_STRING(SDDL_MY_PERMISSIONS, L"D:P(A;; GA;;; SY)(A;; GA;;; BA)(A;; GA;;; WD)");
    DECLARE_CONST_UNICODE_STRING(deviceId, VHID_DEVICE_ID);
    DECLARE_CONST_UNICODE_STRING(deviceLocation, L"Virtual HID\0");
    DECLARE_UNICODE_STRING_SIZE(buffer, MAX_ID_LEN);

    p_device_init = WdfPdoInitAllocate(device);
    if(p_device_init == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    status = WdfPdoInitAssignRawDevice(p_device_init, &GUID_DEVCLASS_VHID);
    if (!NT_SUCCESS(status)) goto Cleanup;

    status = WdfDeviceInitAssignSDDLString(p_device_init, &SDDL_MY_PERMISSIONS);
    if (!NT_SUCCESS(status)) goto Cleanup;

    status = WdfPdoInitAssignDeviceID(p_device_init, &deviceId);
    if (!NT_SUCCESS(status)) goto Cleanup;

    status = RtlUnicodeStringPrintf(&buffer, L"VHID_%02d", instance_no);
    if (!NT_SUCCESS(status)) goto Cleanup;
    instance_no++;

    status = WdfPdoInitAddDeviceText(p_device_init, 
        &buffer, &deviceLocation, 0x409);
    if (!NT_SUCCESS(status)) goto Cleanup;

    WdfPdoInitSetDefaultLocale(p_device_init, 0x409);
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, FORWARD_DEVICE_CONTEXT);
    WdfPdoInitAllowForwardingRequestToParent(p_device_init);

    status = WdfDeviceCreate(&p_device_init, &attributes, &child);
    if (!NT_SUCCESS(status)) goto Cleanup;

    context = GetForwardDeviceContext(child);
    context->instance_no = instance_no;

    device_context = GetDeviceContext(device);
    context->parent_queue = device_context->rep_queue;

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queue_config, WdfIoQueueDispatchSequential);
    queue_config.EvtIoWrite = evt_io_write_for_forward_queue;

    status = WdfIoQueueCreate(child, &queue_config, WDF_NO_OBJECT_ATTRIBUTES, &queue);
    if (!NT_SUCCESS(status)) goto Cleanup;
	
    WDF_DEVICE_STATE_INIT(&device_state);
    device_state.DontDisplayInUI = WdfTrue;
    WdfDeviceSetDeviceState(child, &device_state);

    status = WdfDeviceCreateDeviceInterface(child, &GUID_DEVINTERFACE_VHID, NULL);
    if (!NT_SUCCESS(status)) goto Cleanup;

    status = WdfFdoAddStaticChild(device, child);
    if (!NT_SUCCESS(status)) goto Cleanup;
    device_context->forward_device = child;

    return status;
	
	Cleanup:
    if (p_device_init != NULL) WdfDeviceInitFree(p_device_init);
    if (child)WdfObjectDelete(child);
    return status;
}

VOID evt_io_write_for_forward_queue(
    _In_ WDFQUEUE   Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t     Length)
{
    NTSTATUS status = STATUS_SUCCESS;
    TraceEnterFunc();
    UNREFERENCED_PARAMETER(Length);

    WDFDEVICE parent = WdfIoQueueGetDevice(Queue);
    PFORWARD_DEVICE_CONTEXT context = GetForwardDeviceContext(parent);

    status = WdfRequestForwardToIoQueue(Request, context->parent_queue);
	if(!NT_SUCCESS(status))
        WdfRequestComplete(Request, status);
}

NTSTATUS vhf_initialize(_In_ WDFDEVICE device)
{
    PAGED_CODE();

    VHF_CONFIG vhf_config;
    VHF_CONFIG_INIT(
        &vhf_config,
        WdfDeviceWdmGetDeviceObject(device),
        sizeof(ReportDescriptor), ReportDescriptor
    );
    PDEVICE_CONTEXT device_context = GetDeviceContext(device);
	vhf_config.VhfClientContext = device_context;
    vhf_config.EvtVhfCleanup = vhf_cleanup;
    auto status = VhfCreate(&vhf_config, &device_context->vhf_handle);
    if (!NT_SUCCESS(status)) {
        TraceErrorStatus("VhfCreate", status);
        return status;
    }
    status = VhfStart(device_context->vhf_handle);
    if (!NT_SUCCESS(status)) {
        TraceErrorStatus("VhfStart", status);
        VhfDelete(device_context->vhf_handle, TRUE);
        return status;
    }

    return STATUS_SUCCESS;
}

VOID vhf_cleanup(_In_ PVOID device_context)
{
    TraceEnterFunc();
    ((PDEVICE_CONTEXT)device_context)->vhf_handle = NULL;
}
