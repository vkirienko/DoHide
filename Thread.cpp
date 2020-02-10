#include "stdafx.h"

#include "DoHide.h"
#include "WinList.h"
#include "Thread.h"
#include "options.h"
#include "register.h"
#include "process.h"
#include "globals.h"
#include "TrayIconEnum.h"


const char DOHIDE_HOOK_LIBRARY[] = "DoHideH";
const char DOHIDE_HOOK_PROC[]	 = "HookProc";

bool bHideStill = true;



/*
 *
 *
 */
BOOL CALLBACK PressButtonWindowsProc( HWND hWnd, DWORD lParam )
{
	char szTitle[128];
	GetWndText(hWnd,szTitle,sizeof(szTitle));

	if ( strstr(szTitle,(char*)lParam) )
	{
		SendMessage(hWnd,WM_LBUTTONDOWN,0,0);
		SendMessage(hWnd,WM_LBUTTONUP,0,0);
	}

    return TRUE;
}




/*
 *
 *
 */
void MyBeep( THideByRunInfo* info )
{
	if ( info->m_bBeep )
		MessageBeep(-1);

	info->m_iHideCounter++;
}


/* 
 *
 *
 */
void SetHook(bool fSet)
{
    static HHOOK hhkGetMessage = 0;
    static HANDLE hmodHook = 0;

    if (fSet)
    {
        if (!hmodHook)
        {
            if (!(hmodHook = LoadLibrary(DOHIDE_HOOK_LIBRARY)))
                return;
        }

        if (!hhkGetMessage)
        {
            if ( !(hhkGetMessage = SetWindowsHookEx(WH_CBT,(HOOKPROC)GetProcAddress((HINSTANCE)hmodHook, DOHIDE_HOOK_PROC), (HINSTANCE)hmodHook, 0)) )
                return;
		}
    }
    else
    {
        if (hhkGetMessage)
        {
            UnhookWindowsHookEx(hhkGetMessage);
            hhkGetMessage = NULL;
			hmodHook = NULL;
        }
    }
}


/* 
 *
 *
 */
bool HideIfNotAlreadyHidden( HWND hWnd, bool bHideByRun )
{
	if ( bHideByRun==false )
		return true;

	//  
	//  Проверяем не скрывали ли мы уже его, если только не была нажата 
	//  hotkey для повторного прятания. 
	//
	if ( !bHideStill )
	{
		if( GetProp(hWnd,DOHIDE_PROP1)==HIDDEN_BY_DOHIDE1 )
			return false;
	}

	SetProp(hWnd,DOHIDE_PROP1,HIDDEN_BY_DOHIDE1);
			
	return true;
}


/*
 *
 *
 */
bool IsHideWindow ( HWND hWnd, THideWindow* h )
{
	THideByRunInfo* info = h->info;

	if ( info->m_bDeleteTrayIcon )
		return false;

	char* ptr;
	char  szTitle[128];
	char  szClassName[128];
	GetWndText(hWnd,szTitle,sizeof(szTitle));
	GetClassName(hWnd,szClassName,sizeof(szClassName));

	/*
	 *	Ищем окна по заголовку окна  
	 */
	bool bFindTitle = false;

	if ( info->m_bFilePath )
	{
		/*
		 *	Ищем окна по имени EXE-файла
		 */
		bFindTitle = ProcessList.WindowTitleExist( hWnd, info->GetTitle(0) );
	}
	else
	{
		ptr = ( info->m_bWindowClass )?szClassName:szTitle;

		for ( int i=0; i<=info->GetTitleCount() && !bFindTitle; i++ )
		{
			bFindTitle = CompareTitle ( ptr, info->GetTitle(i), info->m_bHideCaseInsensitive, info->m_bHideByFullTitle );
			/*
			if ( info->m_bHideCaseInsensitive ) 
			{
				if (info->m_bHideByFullTitle) 
				{
					bFindTitle = (lstrcmpi(ptr,info->GetTitle(i))==0);
				}
				else
				{
					char tmp1[MAX_PATH];
					char tmp2[MAX_PATH];
					lstrcpy(tmp1,ptr);
					lstrcpy(tmp2,info->GetTitle(i));
					CharUpper(tmp1);
					CharUpper(tmp2);
					bFindTitle = (strstr(tmp1,tmp2)!=0);
				}
			}
			else
			{
				bFindTitle = (info->m_bHideByFullTitle)?
						(lstrcmp(ptr,info->GetTitle(i))==0):
						(strstr(ptr,info->GetTitle(i))!=0);
			}
			*/
		}
	}

	CHECK_REGISTRATION_2( 5013, IGNORE_COUNTER, 50, ACT_REMOVE_HIDEBYRUN_LIST );

	if ( !bFindTitle )
		return false;

	if ( info->m_bDisabled )
		return false;

	if ( info->m_bHideFirstTime )
	{
		if ( info->m_iHideCounter!=0 )
			return false;
	}

	if ( info->m_bShowWindow )
	{
		ShowOneWindow(hWnd, false);
		return true;
	}

	if ( info->m_bHideIfMinimized )
	{
		if ( !IsIconic(hWnd) )
			return false;
	}


	/*
	 *	Hide actions
	 *
	 */

	if ( info->m_bMaximizeWindow )
	{
		if ( IsWindowVisible(hWnd) && !IsZoomed(hWnd) && IsWindowVisibleOnScreen(hWnd) ) 
		{
			PostMessage(hWnd,WM_SYSCOMMAND,SC_MAXIMIZE,0);
			MyBeep(info);
		}
		return true;
	}

	if ( info->m_bKillProcess )
	{
		KillProcess( hWnd );
		MyBeep(info);
		return true;
	}

	if ( info->m_bCloseWindow )
	{
		//PostMessage(hWnd,WM_QUIT,0,0);
		PostMessage(hWnd,WM_SYSCOMMAND,SC_CLOSE,0);
		MyBeep(info);
		return true;
	}

	if ( info->m_bPressButton )
	{
		EnumChildWindows(hWnd,(WNDENUMPROC)PressButtonWindowsProc,(long)info->m_szPressButtonTitle);
		MyBeep(info);
		return true;
	}

	if ( info->m_bHideAlwaysIfMinimized )
	{
		if ( IsIconic(hWnd) )
		{
			HideOneWindow( hWnd, info->m_bHideToToolbar, TRUE, info->GetFileIcon( ), info->m_bPermanentIcon );
			MyBeep(info);
			return true;
		}
		else 
			return false;
	}

	PRINTF( "%s", HideIfNotAlreadyHidden( hWnd, h->m_bHideByRun )?"true":"false" );
	PRINTF( "%s", info->m_bHideAndIfMinimized?"true":"false" );
	PRINTF( "%s", IsIconic(hWnd)?"true":"false" );

	if ( HideIfNotAlreadyHidden( hWnd, h->m_bHideByRun )==true || 
		 (info->m_bHideAndIfMinimized && IsIconic(hWnd)) 
		)
	{
		if ( info->m_bMoveToOutside )
		{
			RECT r;
			GetWindowRect(hWnd,&r);
			int w = r.right-r.left;
			int h = r.bottom-r.top;
			MoveWindow(hWnd, -w+1, -h+1, w, h, 1 );
		}
		else
		{
			HideOneWindow( 
				hWnd, 
				info->m_bHideToToolbar, 
				info->m_bHideAndIfMinimized, 
				info->GetFileIcon( ),
				info->m_bPermanentIcon
			);
		}

		MyBeep(info);
		return true;
	}

	return false;
} 


/* 
 *
 *
 */
BOOL CALLBACK HideWindowsProc( HWND hWnd, DWORD lParam )
{
	if ( (UseWindow( hWnd,true ) && IsWindowVisible(hWnd)) || 
		 (lParam!=0 && ((THideWindow*)lParam)->m_bHideByRun==false) 
		)
	{
		if ( lParam==NULL )
		{
			for ( int i=0; i<HideTitleList.GetCount(); i++ )
			{
				THideByRunInfo* info = (THideByRunInfo*)HideTitleList.GetInfo(i);
				if ( info==0 || info->Empty() )
					continue;
				THideWindow hide(info,true);
				if ( IsHideWindow( hWnd, &hide )==true )
					return TRUE;
			}
		}
		else
		{
			if ( IsHideWindow( hWnd, (THideWindow*)lParam )==true )
				return TRUE;
		}

		if ( uKey[7] && IsIconic(hWnd) && !GetProp(hWnd,DOHIDE_PROP) )
		{
			HideOneWindow(hWnd,!uKey[15],TRUE);
		}
	}

	return TRUE;
}


/*
 *
 *
 */
void CALLBACK TimerCallbackProc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime )
{
/*
	DWORD dwStartTime = GetTickCount();
	for ( int z=0; z<100;z++)
	{
*/


	TrayMinimizedList.CheckWindows();

	EnumWindows( (WNDENUMPROC)HideWindowsProc, 0 );

	for ( int i=0; i<HideTitleList.GetCount(); i++ )
	{
		THideByRunInfo* info = (THideByRunInfo*)HideTitleList.GetInfo(i);
		if ( info==0 || info->Empty() )
			continue;
		HWND hWnd = FindWindow( 0, info->GetTitle(0) );
		if ( hWnd )
		{
			THideWindow hide(info,false);
			HideWindowsProc( hWnd, (long)&hide );
		}
	}

	if ( HideTitleList.GetDeleteFromTrayCount() )
		EnumTrayIcons( );



/*	
	}
	PRINTF("%d",GetTickCount()-dwStartTime);
*/
	bHideStill = false;
}
