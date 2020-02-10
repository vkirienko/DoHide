#ifndef ____DOHIHEH_H_______
#define ____DOHIHEH_H_______


#define CM_PID         0
#define CM_ACTIVATE    1
#define CM_MINIMIZE    2

struct WND_INFO
{
	UINT  m_Command;
	HWND  m_hWnd;
	DWORD m_PID;
	char  m_szModuleName[MAX_PATH];
};

#endif