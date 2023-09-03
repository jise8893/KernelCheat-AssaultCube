#include "UserMode.h"

int main()
{
	HANDLE handle;
	
	
	const wchar_t* DriverName = L"\\\\.\\kernelCheat";
	UserModeController* controller = new UserModeController();
	controller->InitDriver(DriverName);


	controller->GetPid();

	controller->GetInAddress(0x0017E0A8 ,0x4);
	
	controller->WriteValue(0xEC, 1000, 0x4);
	delete controller;


	return 0;
}