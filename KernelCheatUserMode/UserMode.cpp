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

int main()
{
	HANDLE handle;
	DWORD Bytes;
	handle = CreateFile(L"\\\\.\\kernelCheat", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	KERNEL_BASE_REQUEST kBase; 
	
	int errCode=DeviceIoControl(handle, IOCTL_GET_REQUEST_EVENT, &kBase, sizeof(kBase), &kBase, sizeof(kBase), &Bytes, NULL);
	if (errCode != 0)
	{
		cout << GetLastError() << endl;
	}
	printf("pid : %lld address: %x", kBase.pid, kBase.Address);


	_KERNEL_READ_REQUEST kRead; 
	kRead.Size = 0x4; 
	kRead.Address = 0x0017E0A8+kBase.Address;
	kRead.Response = 0;

	DeviceIoControl(handle, IOCTL_READ_REQUEST_EVENT, &kRead, sizeof(kRead), &kRead, sizeof(kRead), &Bytes, NULL);  

	ULONG ClientAddress = kRead.Response;
	
	KERNEL_WRITE_REQUEST kWrite; 
	kWrite.Address = ClientAddress + 0xEC;
	kWrite.Size = 0x4;
	kWrite.ProcessId =(ULONGLONG) kBase.pid; 
	kWrite.Value = 1000;

	DeviceIoControl(handle, IOCTL_WRITE_REQUEST_EVENT, &kWrite, sizeof(kWrite), &kWrite, sizeof(kWrite), &Bytes, NULL);
	
//	cout <<"value "+kRead.Response << endl;

	return 0;
}