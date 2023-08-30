
#include "KernelCheatDriver.h"
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	UNICODE_STRING dev;
	UNICODE_STRING dos;
	
	PDEVICE_OBJECT pDevice=NULL; 
	RtlInitUnicodeString(&dev, L"\\Device\\kernelCheat");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\kernelCheat");

	PsSetLoadImageNotifyRoutine(ImageLoadCallback);


	NTSTATUS status;
	status = IoCreateDevice(DriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDevice);
	IoCreateSymbolicLink(&dos, &dev);
	DriverObject->DriverUnload = UnloadDriver;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall; 
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	return STATUS_SUCCESS;


}
