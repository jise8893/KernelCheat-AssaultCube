#pragma once
#include <Windows.h>
#include <cstdio>
#include <iostream>
using namespace std;
#define IOCTL_GET_REQUEST_EVENT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800,METHOD_BUFFERED,FILE_ANY_ACCESS) //0x800-0xfff 
#define IOCTL_READ_REQUEST_EVENT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801,METHOD_BUFFERED,FILE_ANY_ACCESS) //0x800-0xfff 
#define IOCTL_WRITE_REQUEST_EVENT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802,METHOD_BUFFERED,FILE_ANY_ACCESS) //0x800-0xfff 


typedef struct _KERNEL_READ_REQUEST
{
	ULONGLONG ProcessId;

	ULONGLONG Address;
	ULONGLONG Response;
	ULONGLONG Size;

} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	ULONGLONG ProcessId;

	ULONGLONG Address;
	ULONGLONG Value;
	ULONGLONG Size;

} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

typedef struct _KERNEL_BASE_REQUEST
{
	HANDLE pid;
	ULONGLONG Address;
}KERNEL_BASE_REQUEST, * PKERNEL_BASE_REQUEST;

class UserModeController
{
private:
	HANDLE handle;
public:
	KERNEL_BASE_REQUEST kBase;
	KERNEL_READ_REQUEST kRead;
	KERNEL_WRITE_REQUEST kWrite; 
	ULONG ClientAddress;

	bool InitDriver(const wchar_t * wstr);
	void GetPid();
	void GetInAddress(ULONGLONG address, SIZE_T size);
	void WriteValue(ULONGLONG offset, ULONGLONG value,ULONGLONG size);
};