#include <windows.h>
#include <stdio.h>
#include "DoHideH.h"

struct WND_INFO WndInfo;
COPYDATASTRUCT cds = { 0, sizeof(WndInfo), &WndInfo };

const char DOHIDE_PROP[]  = "DoHideH";
const char DOHIDE_PROP1[] = "DoHideHR";
const char DOHIDE_TITLE[] = "DoHide"; 

/* 
 *
 *
 */
BOOL APIENTRY DllMain( PVOID hModule, ULONG ulReason, PCONTEXT pctx )
{ 
    if ( ulReason == DLL_PROCESS_ATTACH ) 
	{
		DWORD dwResult;
		HWND hDohideWnd = FindWindow(0,DOHIDE_TITLE);

		WndInfo.m_Command  = CM_PID;
		WndInfo.m_hWnd     = 0;
		WndInfo.m_PID      = GetCurrentProcessId();
		GetModuleFileName( 0, WndInfo.m_szModuleName, sizeof(WndInfo.m_szModuleName) );

		if ( hDohideWnd )
			SendMessageTimeout( hDohideWnd, WM_COPYDATA, (WPARAM)0, (LPARAM)&cds, SMTO_NORMAL, 100, &dwResult );
		
	    return TRUE;
    }
 
	return TRUE; 
}


/* 
 *
 *
 */
LRESULT CALLBACK HookProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	if ( nCode<0 )
		return CallNextHookEx(NULL, nCode, wParam, lParam);

	if ( nCode==HCBT_ACTIVATE )
	{
		HWND hWnd = (HWND)wParam;
		if ( GetProp(hWnd,DOHIDE_PROP) || GetProp(hWnd,DOHIDE_PROP1) )
		{
			DWORD dwResult;
			HWND hDohideWnd   = FindWindow(0,DOHIDE_TITLE);
			WndInfo.m_Command = CM_ACTIVATE;
			WndInfo.m_hWnd    = hWnd;
			if ( hDohideWnd )
				SendMessageTimeout( hDohideWnd, WM_COPYDATA, (WPARAM)0, (LPARAM)&cds, SMTO_NORMAL, 100, &dwResult );
		}
	}

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


