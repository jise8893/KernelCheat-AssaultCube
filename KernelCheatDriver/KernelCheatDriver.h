#pragma once
#include <ntifs.h>
#include <ntdef.h>
#include <windef.h>
#include <ntstrsafe.h>
#include <wdm.h>
extern "C" __declspec (dllimport)
NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);


//#pragma comment(lib, "Ntoskrnl.lib")
#define IOCTL_READ_REQUEST_EVENT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801,METHOD_BUFFERED,FILE_ANY_ACCESS) //0x800-0xfff
#define IOCTL_WRITE_REQUEST_EVENT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802,METHOD_BUFFERED,FILE_ANY_ACCESS) //0x800-0xfff
#define IOCTL_GET_REQUEST_EVENT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800,METHOD_BUFFERED,FILE_ANY_ACCESS) //0x800-0xfff





typedef struct _KERNEL_BASE_REQUEST
{
	HANDLE pid;
	ULONGLONG Address;
}KERNEL_BASE_REQUEST, * PKERNEL_BASE_REQUEST;

typedef struct _KERNEL_READ_REQUEST
{
	ULONGLONG ProcessId;

	ULONG Address;
	ULONGLONG Response;
	SIZE_T Size;

} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	ULONGLONG ProcessId;

	ULONG Address;
	ULONGLONG Value;
	SIZE_T Size;

} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

KERNEL_BASE_REQUEST mInfoBase;

void ImageLoadCallback(PUNICODE_STRING FullImageName,
	HANDLE ProcessId,
	PIMAGE_INFO ImageInfo)
{

	if (wcsstr(FullImageName->Buffer, L"ac_client.exe"))
	{
		DbgPrint(" Process id : %lld", ProcessId);
		mInfoBase.pid = ProcessId;
		mInfoBase.Address = (ULONGLONG)ImageInfo->ImageBase;
	}
}

void UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unload routine called.\n");

	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	IoDeleteDevice(pDriverObject->DeviceObject);
	//IoDeleteSymbolicLink(pDriverObject->DeviceObject);
}
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS Status;
	ULONG BytesIo = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	Status = STATUS_UNSUCCESSFUL;
	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	if (ControlCode == IOCTL_GET_REQUEST_EVENT)
	{
		PKERNEL_BASE_REQUEST OutPut = (PKERNEL_BASE_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		*OutPut = mInfoBase;
		Status = STATUS_SUCCESS;
		BytesIo = sizeof(*OutPut);

	}
	else if (ControlCode == IOCTL_READ_REQUEST_EVENT)
	{
		PKERNEL_READ_REQUEST ReadInput = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		//PKERNEL_READ_REQUEST ReadOutput = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		PEPROCESS process;

		if (NT_SUCCESS(PsLookupProcessByProcessId(mInfoBase.pid, &process)))
		{
			SIZE_T Result;
			MmCopyVirtualMemory(process, (PVOID)(ReadInput->Address), PsGetCurrentProcess(), &ReadInput->Response, ReadInput->Size, KernelMode, &Result);
			
			BytesIo = sizeof(KERNEL_READ_REQUEST);
			Status = STATUS_SUCCESS;
		}
	}
	else if (ControlCode == IOCTL_WRITE_REQUEST_EVENT)
	{

		PKERNEL_WRITE_REQUEST WriteInput = (PKERNEL_WRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		PKERNEL_WRITE_REQUEST WriteOutput = (PKERNEL_WRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		PEPROCESS process;
		SIZE_T Result;
		if (NT_SUCCESS(PsLookupProcessByProcessId(mInfoBase.pid, &process)))
		{

			if (NT_SUCCESS(MmCopyVirtualMemory(PsGetCurrentProcess(), (PVOID)&WriteOutput->Value, process, (PVOID)WriteInput->Address, WriteInput->Size, KernelMode, &Result)))
			{
				Status = STATUS_SUCCESS;
			}
			else {
				Status = STATUS_UNSUCCESSFUL;
			}
			BytesIo = sizeof(KERNEL_WRITE_REQUEST);
		}
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIo;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}