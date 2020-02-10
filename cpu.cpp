#include "stdafx.h"

void CpuTrayMessage(DWORD dwMessage, int iCpuUsage);

bool bExitCpuThread;

DWORD WINAPI CpuThreadProc( PVOID lpParameter )
{
	while( !bExitCpuThread )
	{
		Sleep(1000);
		CpuTrayMessage(NIM_MODIFY,0);
	}
	return 0;
}


void StartCpuThread()
{
	DWORD dwThreadId;
	bExitCpuThread = false;
	CreateThread( 0, 0, CpuThreadProc, 0, 0, &dwThreadId );
}


void StopCpuThread()
{
	bExitCpuThread = true;
}