#include "UserMode.h"

bool UserModeController::InitDriver(const wchar_t *wstr)
{
	handle = CreateFile(wstr, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); 
	if (handle == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

void UserModeController::GetPid()
{
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
}
