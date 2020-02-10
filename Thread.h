#ifndef __THREAD_H__
#define __THREAD_H__

#include "DoHideH\\DoHideH.h"
#include "WinList.h"


/*
 *
 *
 */
class THideWindow
{
public:
	THideWindow(THideByRunInfo* inf, bool HideByRun=false) 
	{ 
		info = inf; 
		m_bHideByRun = HideByRun; 
	}
	THideByRunInfo* info;
	bool            m_bHideByRun; 
};

extern bool	bHideStill;
extern void SetHook(bool);

extern void CALLBACK TimerCallbackProc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime ); 
extern BOOL CALLBACK HideWindowsProc( HWND hWnd, DWORD lParam );


#endif