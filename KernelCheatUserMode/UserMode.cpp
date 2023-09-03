#include "UserMode.h"

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


bool UserModeController::InitDriver(const wchar_t *wstr)
{
	handle = CreateFile(wstr, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); 
	if (handle == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

void UserModeController::GetPid()
{
	HANDLE handle;
	DWORD Bytes;
	int errCode = DeviceIoControl(handle, IOCTL_GET_REQUEST_EVENT, &kBase, sizeof(kBase), &kBase, sizeof(kBase), &Bytes, NULL); 
	if (errCode != 0)
	{
		cout << GetLastError() << endl;
	}
	printf("pid : %lld address: %x\n", kBase.pid, kBase.Address); 
}

void UserModeController::GetInAddress(ULONGLONG address,SIZE_T size)
{
	DWORD Bytes; 
	kRead.Size = size;
	kRead.Address = address + kBase.Address; 
	kRead.Response = 0;

	DeviceIoControl(handle, IOCTL_READ_REQUEST_EVENT, &kRead, sizeof(kRead), &kRead, sizeof(kRead), &Bytes, NULL);  
	ClientAddress = kRead.Response; 
}
	
void UserModeController::WriteValue(ULONGLONG offset, ULONGLONG value,ULONGLONG size)
{
	DWORD Bytes;
	kWrite.Address = ClientAddress + offset;
	kWrite.Size = size;
	kWrite.ProcessId = (ULONGLONG)kBase.pid;
	kWrite.Value = value;

	DeviceIoControl(handle, IOCTL_WRITE_REQUEST_EVENT, &kWrite, sizeof(kWrite), &kWrite, sizeof(kWrite), &Bytes, NULL);
	
//	cout <<"value "+kRead.Response << endl;

	return 0;
}

UserModeController::~UserModeController()
{
	CloseHandle(handle);
}
