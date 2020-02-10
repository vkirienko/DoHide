#include "StdAfx.h"

#include "Thread.h"
#include "DoHide.h"
#include "DoHideH\\DoHideH.h"
#include "WinList.h"
#include "Options.h"
#include "hotkeys.h"
#include "register.h"
#include "defend.h"
#include "registry.h"
#include "bitmap.h"
#include "Globals.h"
#include "shell.h"
#include "menu.h"
#include "dde.h"
#include "TrayIconEnum.h"
#include "language.h"
#include "cpu.h"

#include "resource.h"


//---------------------------------------------------------------------------
// Global to everybody...
//---------------------------------------------------------------------------
const char DOHIDE_PROP[]        = "DoHideH";
const char DOHIDE_PROP1[]       = "DoHideHR";
const char DOHIDE_PROP2[]       = "PermanentIcon";
const HANDLE HIDDEN_BY_DOHIDE   = (HANDLE)111; 
const HANDLE HIDDEN_BY_DOHIDE_R = (HANDLE)222; // Restore after show
HANDLE HIDDEN_BY_DOHIDE1        = (HANDLE)333; 

const char SHELL_OPEN[]			= "open";
const char SHELL_CONTROL[]		= "control.exe";
const char DESK_CPL[]			= "desk.cpl";

const char DOHIDE[]				  = "DoHide"; 
const char SECTION_DOHIDE[]       = "Software\\MrKirienko\\DoHide\\";
const char PROGRAM_MANAGER_NAME[] = "Program Manager";

#define STR( n, s )  const char STR##n[] = (s);
#define PTR( n )     (char*)(STR##n)

STR( 0,  "UseRussian" )
STR( 1,  "NotUseHideByName" )	// Unused since 3.0 version
STR( 2,  "ShowDesktopIcons" )
STR( 3,  "ShowInvisible" )
STR( 4,  "Version" )
STR( 5,  "HideToToolbar" )
STR( 6,  "ResponseTime" )
STR( 7,  "MinimizeToTray" )
STR( 8,  "PropertyPanel" )
STR( 9,  "AddToStartup" )
STR( 10, "NoDesktopMenu" )
STR( 11, "NoRecent" )
STR( 12, "NoDoHideIcon" )
STR( 13, "PermanentTrayIcon" )
STR( 14, "NoFavorites" )
STR( 15, "MinimizeCompletely" )
STR( 16, "NoDialUp" )
STR( 17, "ProtectByPassword" )
STR( 18, "NoControlPanel" )
STR( 19, "NoPrinters" )
STR( 20, "NoCPU" )

char* SECTION[] = { 
	PTR(0),  PTR(1),  PTR(2),  PTR(3),  PTR(4),  PTR(5),  PTR(6),  PTR(7),  PTR(8), 
	PTR(9),  PTR(10), PTR(11), PTR(12), PTR(13), PTR(14), PTR(15), PTR(16), PTR(17),
	PTR(18), PTR(19), PTR(20)
};

int DEFAULT[COUNT_KEYS] = { 
	0,0,1,0,
	DOHIDE_VERSION,0,1000,0,
	0,0,0,1,0,0,1,0,1,
	PWD_NO_PASSWORD,1,1,0 
};

const char _winMM_dll[]		= "winmm.dll";
const char _mciSendString[]	= "mciSendStringA";
const char _OpenTray[]		= "SET CDAUDIO DOOR OPEN WAIT";
const char _CloseTray[]		= "SET CDAUDIO DOOR CLOSED WAIT";
const char SZ_PASSWORD[]	= "Password";

const char PRM_OPTIONS1[]  = "/O";
const char PRM_OPTIONS2[]  = "/o";

const char _SE_SHUTDOWN_NAME[] = SE_SHUTDOWN_NAME;
const char _REGSTR_PATH_RUN[]  = REGSTR_PATH_RUN;

      
HINSTANCE hInst;
HINSTANCE hResourceInst;
HWND      hMainDlg;
UINT	  uKey[COUNT_KEYS];
bool	  bDoHideIconExist;
 
bool      bRestart = false;

#ifdef NEW_DEFENCE
DWORD     dwAppStartTime = 0;
DWORD     dwAppCurrTime;
#endif




/*
 *
 *
 */
void LoadConfiguration()
{
	for ( int i=0; i<COUNT_KEYS; i++ ) 
	{
		ReadKey( HKEY_CURRENT_USER, SECTION_DOHIDE, SECTION[i], uKey[i], DEFAULT[i] );
	}
}

void StoreConfiguration( bool bSavePassword )
{
	for ( int i=0; i<COUNT_KEYS; i++ ) 
	{	
		WriteKey( HKEY_CURRENT_USER, SECTION_DOHIDE, SECTION[i], uKey[i] );
	}

	if ( bSavePassword )
		WriteKey( HKEY_CURRENT_USER, SECTION_DOHIDE, SZ_PASSWORD, (BYTE*)&Security->m_hash, sizeof(Security->m_hash) );

	if ( uKey[9] )
	{
		char s[MAX_PATH];
		char s1[MAX_PATH];
		GetModuleFileName( 0, s, sizeof(s) );
		lstrcpy(s1,s);
		s1[lstrlen(s1)-10] = 0;
		CreateStartupLink( s, DOHIDE, "", s1 );
	}
	else
	{
		DeleteStartupLink( DOHIDE );
	}
	DeleteKey( HKEY_LOCAL_MACHINE, _REGSTR_PATH_RUN, DOHIDE );
}


/*
 *
 *
 */
bool isProgramManager ( char* s )
{
	return !(lstrcmp(s,PROGRAM_MANAGER_NAME));
}


/*
 *
 *
 */
void ReplaceDesktopIcons( char* s )
{
	if ( isProgramManager(s) )
		LoadStr( IDS_DESKTOP_ICONS, s, 30 );
}


//
//
//
BOOL CALLBACK ShowHideAllWindowsProc( HWND hWnd, DWORD lParam )
{
	if ( !UseWindow ( hWnd ) )
		return TRUE;
	
	PRINTF("hWnd: %08X   IsZoomed: %d", hWnd, IsZoomed(hWnd) );

	switch ( LOWORD(lParam) )
	{
		 case HKID_SHOW_ALL:		 
			 ShowOneWindow( hWnd, false, (HIWORD(lParam)==1) );
		
		_CASE HKID_MINIMIZE_ALL:	 
			 if ( IsWindowVisible(hWnd) && !IsIconic(hWnd) ) 
				 PostMessage( hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0 ); 
		
		_CASE HKID_MAXIMIZE_ALL:	 
			 if ( IsWindowVisible(hWnd) && !IsZoomed(hWnd) && IsWindowVisibleOnScreen(hWnd) ) 
				 PostMessage( hWnd,WM_SYSCOMMAND,SC_MAXIMIZE,0 ); 
		
		_CASE HKID_RESTORE_ALL:		 
			 PostMessage( hWnd,WM_SYSCOMMAND,SC_RESTORE,0 ); 

		_CASE HKID_HIDE_ALL:		  
		 case HKID_HIDE_ALL_TOOLBAR: 
		{
			char s[128];
			GetWndText( hWnd,s,sizeof(s) );
			if ( !isProgramManager(s) ) 
				HideOneWindow( hWnd, (LOWORD(lParam)==HKID_HIDE_ALL_TOOLBAR) ); 
		}
		_CASE HKID_CLOSE_ALL_WINDOWS:
			PostMessage(hWnd,WM_SYSCOMMAND,SC_CLOSE,0);
	}

    return TRUE;
}

/*
 *
 *
 */
BOOL CALLBACK FillMenuWindowsProc( HWND hWnd, DWORD lParam )
{
	if ( UseWindow ( hWnd ) )
	{
		MENUITEMINFO mInfo;
		mInfo.cbSize     = sizeof(MENUITEMINFO);
		mInfo.fMask      = MIIM_ID|MIIM_STATE|MIIM_TYPE|MIIM_DATA;
		mInfo.fType      = MFT_OWNERDRAW;
		mInfo.fState     = (IsWindowVisible(hWnd)?MFS_CHECKED:0); 
		mInfo.hSubMenu   = 0; 
        mInfo.dwItemData = CustomMenu.AddItem( (int)hWnd, MIT_HWND );
		mInfo.wID        = WM_EXECUTE_CUSTOM_MENU + mInfo.dwItemData; 
		InsertMenuItem(CustomMenu.GetMenu(), 2+mInfo.dwItemData, TRUE,  &mInfo);
	}
    return TRUE;
}


/*
 *
 *
 */
void KillProcess( HWND hWnd )
{
	DWORD dwProcessId;
	GetWindowThreadProcessId( hWnd, &dwProcessId ); 
	HANDLE hProcess = OpenProcess (PROCESS_TERMINATE, FALSE, dwProcessId);
	if (!hProcess)
		return;
	TerminateProcess( hProcess, 0 );
	CloseHandle (hProcess);
}



/*
 *
 *
 */
BOOL CALLBACK HideTopWindowProc( HWND hWnd, DWORD lParam )
{
	if ( !UseWindow ( hWnd ) )
	    return TRUE;

	switch ( lParam ) 
	{
		case  HKID_HIDE_CURR:		  HideOneWindow ( hWnd, 0 );
		_CASE HKID_HIDE_CURR_TOOLBAR: HideOneWindow ( hWnd, 1 );
		_CASE HKID_MINIMIZE_CURRENT:  PostMessage( hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0 );
		_CASE HKID_MAXIMIZE_CURRENT:  PostMessage( hWnd,WM_SYSCOMMAND,SC_MAXIMIZE,0 );
		_CASE HKID_KILL_PROCESS:      KillProcess( hWnd );
		_CASE HKID_SET_WINDOW_TOPMOST:
			{
			LONG style = GetWindowLong(hWnd, GWL_EXSTYLE);
			SetWindowPos( hWnd, (style&WS_EX_TOPMOST)?HWND_NOTOPMOST:HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 
			}
		_CASE HKID_MAXIMIZE_X:
		 case HKID_MAXIMIZE_Y:
		 case HKID_MOVE_TOP:
		 case HKID_MOVE_BOTTOM:
		 case HKID_MOVE_LEFT:				
		 case HKID_MOVE_RIGHT:				
			{
			RECT rect;
			GetWindowRect( hWnd, &rect );
			switch (lParam)
			{
				 case HKID_MAXIMIZE_X:
					SetWindowPos( hWnd, 0, 0,rect.top,GetSystemMetrics(SM_CXSCREEN),rect.bottom-rect.top, SWP_NOOWNERZORDER ); 
				_CASE HKID_MAXIMIZE_Y:
					SetWindowPos( hWnd, 0, rect.left,0,rect.right-rect.left,GetSystemMetrics(SM_CYSCREEN), SWP_NOOWNERZORDER ); 
				_CASE HKID_MOVE_TOP:
					SetWindowPos( hWnd, 0, rect.left,0,rect.right-rect.left,rect.bottom-rect.top, SWP_NOOWNERZORDER ); 
				_CASE HKID_MOVE_BOTTOM:
					SetWindowPos( hWnd, 0, rect.left,GetSystemMetrics(SM_CYSCREEN)-rect.bottom-rect.top,rect.right-rect.left,GetSystemMetrics(SM_CYSCREEN), SWP_NOOWNERZORDER ); 
				_CASE HKID_MOVE_LEFT:				
					SetWindowPos( hWnd, 0, 0,rect.top,rect.right-rect.left,rect.bottom, SWP_NOOWNERZORDER ); 
				_CASE HKID_MOVE_RIGHT:				
					SetWindowPos( hWnd, 0, GetSystemMetrics(SM_CXSCREEN)-rect.right-rect.left,rect.top,GetSystemMetrics(SM_CXSCREEN),rect.bottom, SWP_NOOWNERZORDER ); 
			}
			}
	}

	CHECK_REGISTRATION_2( 14, CLEAR_COUNTER, 55, ACT_CLEAR_HOTKEY );

	return FALSE;
}


//
//
//
void TrayMessage(DWORD dwMessage, PSTR pszTip)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = 0;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON;
	tnd.hIcon		     = LoadIcon(hInst,MAKEINTRESOURCE(IDI_DOHIDE));

	lstrcpy(tnd.szTip, pszTip);
	Shell_NotifyIcon(dwMessage, &tnd);

	if ( dwMessage==NIM_ADD )
		bDoHideIconExist = true;
	if ( dwMessage==NIM_DELETE )
		bDoHideIconExist = false;
}


/*
 *
 *
 */
void DesktopTrayMessage(DWORD dwMessage)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = 10;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON_DESKTOP;
	tnd.hIcon		     = LoadIcon(hInst,MAKEINTRESOURCE(IDI_DESKTOP));
	
	LoadStr( IDS_DESKTOP,tnd.szTip,sizeof(tnd.szTip));
	Shell_NotifyIcon(dwMessage, &tnd);
}


/*
 *
 *
 */
void RecentTrayMessage(DWORD dwMessage)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = 11;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON_RECENT;
	tnd.hIcon		     = LoadIcon(hInst,MAKEINTRESOURCE(IDI_RECENT));
	
	LoadStr( IDS_RECENT,tnd.szTip,sizeof(tnd.szTip));
	Shell_NotifyIcon(dwMessage, &tnd);
}


/*
 *
 *
 */
void FavoritesTrayMessage(DWORD dwMessage)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = 12;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON_FAVORITES;
	tnd.hIcon		     = LoadIcon(hInst,MAKEINTRESOURCE(IDI_IEXPLORER));
	
	LoadStr( IDS_IEXPLORER,tnd.szTip,sizeof(tnd.szTip));
	Shell_NotifyIcon(dwMessage, &tnd);
}


/*
 *
 *
 */
void DialupTrayMessage(DWORD dwMessage)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = 13;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON_DIALUP;
	tnd.hIcon		     = LoadIcon(hInst,MAKEINTRESOURCE(IDI_DIAL));
	
	LoadStr( IDS_DIALUP,tnd.szTip,sizeof(tnd.szTip));
	Shell_NotifyIcon(dwMessage, &tnd);
}


/*
 *
 *
 */
void ControlPanelTrayMessage(DWORD dwMessage)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = 14;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON_CONTROL;
	tnd.hIcon		     = LoadIcon(hInst,MAKEINTRESOURCE(IDI_CONTROL_PANEL));
	
	LoadStr( IDS_CONTROL_PANEL,tnd.szTip,sizeof(tnd.szTip));
	Shell_NotifyIcon(dwMessage, &tnd);
}

/*
 *
 *
 */
void PrintersTrayMessage(DWORD dwMessage)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = 15;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON_PRINTERS;
	tnd.hIcon		     = LoadIcon(hInst,MAKEINTRESOURCE(IDI_PRINTERS));
	
	LoadStr( IDS_PRINTERS,tnd.szTip,sizeof(tnd.szTip));
	Shell_NotifyIcon(dwMessage, &tnd);
}

/*
 *
 *
 */
void CpuTrayMessage(DWORD dwMessage, int iCpuUsage)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = 16;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON_CPU;
	tnd.hIcon		     = LoadIcon(hInst,MAKEINTRESOURCE(IDI_CPU));
	tnd.hIcon			 = CpuIcon(tnd.hIcon,32,32,iCpuUsage);
	
	LoadStr( IDS_CPU,tnd.szTip,sizeof(tnd.szTip));
	Shell_NotifyIcon(dwMessage, &tnd);

	if ( dwMessage==NIM_ADD )
		StartCpuThread();
	if ( dwMessage==NIM_DELETE )
		StopCpuThread();
}

/*
 *
 *
 */
BOOL CALLBACK CountHiddenWindowsProc( HWND hwnd, DWORD lParam )
{
	if ( GetProp(hwnd,DOHIDE_PROP) )
	{
		(*(int*)lParam)++;
	}
    return TRUE;
}

//
//
//
void StateChange()
{
	char s[40];
	char s1[40];
	int  hidden = 0;
	EnumWindows( (WNDENUMPROC)CountHiddenWindowsProc, (LPARAM)&hidden );

	if ( hidden==0 )
	{
		LoadStr( IDS_NO_WINDOW, s, sizeof(s) );
	}
	else
	{
		if ( uKey[0]==1 ) // если язык русский
		{
			char s2[40];
			int ID;

			if ( hidden==1 ) ID = IDS_LANG1; else
			if ( hidden>=2 && hidden<=4 ) ID = IDS_LANG2; else 
			ID = IDS_LANG;

			LoadStr(IDS_HIDDEN_WINDOWS, s1, sizeof(s1));
			LoadStr(ID, s2, sizeof(s2));
			lstrcat(s1,s2);
		}
		else
		{
			LoadStr(IDS_HIDDEN_WINDOWS, s1, sizeof(s1));
		}
		wsprintf( s, s1, hidden );
	}

	TrayMessage(NIM_MODIFY, s);
}

//
//
//
bool UseWindow ( HWND hWnd, bool bIgnoreTitleLength )
{
	if ( GetProp(hWnd,DOHIDE_PROP) || GetProp(hWnd,DOHIDE_PROP1) )
		return true;

	if ( hWnd==hMainDlg )
		return false;

	int  title_len = GetWndTextLen(hWnd);
	if ( title_len==0 && bIgnoreTitleLength==false )
		return false;

	BOOL is_child = GetWindowLong(hWnd,GWL_STYLE)&WS_CHILD;
	if ( is_child==TRUE )
		return false;

	BOOL invisible = uKey[3]?0:!IsWindowVisible(hWnd);
	if ( invisible )
		return false;

	HWND parent = GetParent(hWnd);
	if ( parent )
	if ( parent==GetWindow(hWnd,GW_OWNER) && !IsWindowVisible(parent) )
		parent = 0;
	if ( parent )
		return false;

	if ( !uKey[2] )
	{
		BOOL isPM = 0;
		char s[255];
		GetWndText( hWnd, s, sizeof(s) );
		if ( isProgramManager(s) )
			return false;
	}

	RECT r;
	GetWindowRect(hWnd,&r);
	bool size = (r.bottom-r.top!=0 && r.right-r.left!=0);
	if ( size==0 )
		return false;

	return true;
/*
	int  title_len = GetWndTextLen(hWnd);
	BOOL is_child  = GetWindowLong(hWnd,GWL_STYLE)&WS_CHILD;
	BOOL invisible = uKey[3]?0:!IsWindowVisible(hWnd);
	HWND parent    = GetParent(hWnd);
	if ( parent )
	if ( parent==GetWindow(hWnd,GW_OWNER) && !IsWindowVisible(parent) )
		parent = 0;

	BOOL isPM = 0;
	if ( !uKey[2] )
	{
		char s[255];
		GetWndText( hWnd, s, sizeof(s) );
		isPM = isProgramManager(s);
	}

	RECT r;
	GetWindowRect(hWnd,&r);
	bool size = (r.bottom-r.top!=0 && r.right-r.left!=0);

	BOOL ret = ( 
		!isPM && !invisible && !parent && !is_child && (title_len||bIgnoreTitleLength) && 
		hWnd!=hMainDlg && size
		);

	return ret || GetProp(hWnd,DOHIDE_PROP) || GetProp(hWnd,DOHIDE_PROP1);
*/
}


//
//
//
void ShowOneWindow ( HWND hWnd, bool bShowConsoleWindow, bool bRemoveTrayIcon )
{
	static HWND hShowedWindow = 0;	// Workaround Win95 and console window bug ????

	if ( hShowedWindow==hWnd )
		return;

	if ( IsWindow(hWnd) && !IsWindowVisible(hWnd) )
	{
		if ( !CheckPassword( PWD_SHOW_WINDOW ) )
			return;
	}

	hShowedWindow = hWnd;

	if ( GetProp(hWnd,DOHIDE_PROP)==HIDDEN_BY_DOHIDE_R )
	{
		if ( !IsWindowVisible(hWnd) )
				SendMessage( hWnd, WM_SYSCOMMAND, SC_RESTORE, 0 );
		if ( bShowConsoleWindow==true )
			SetForegroundWindow(hWnd);
	}

	if ( !(uKey[13] || GetProp(hWnd,DOHIDE_PROP2)) || bRemoveTrayIcon )
	{
		NOTIFYICONDATA tnd;
		tnd.cbSize = sizeof(NOTIFYICONDATA);
		tnd.hWnd   = hMainDlg;
		tnd.uID	   = (UINT)hWnd;
		tnd.uFlags = NIF_MESSAGE;
		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}

	if ( !IsWindowVisible(hWnd) )
	{
		ShowWindow( hWnd, SW_SHOWNORMAL ); 
		HWND owner = GetWindow(hWnd,GW_OWNER);
		if ( owner ) 
			ShowWindow( owner, SW_SHOWNORMAL ); 
		SetForegroundWindow(hWnd);	
		BringWindowToTop(hWnd);
		SetFocus(hWnd);
	
		ShowWindow( hMainDlg, SW_HIDE ); 
	}

	if ( !IsIconic(hWnd) )
		RemoveProp(hWnd,DOHIDE_PROP);

	hShowedWindow = 0;
}
 


//
//
//
void HideOneWindow ( HWND hWnd, BOOL bHideToToolbar, BOOL bNeedRestore, HICON hIcon, bool bPermanentTrayIcon )
{
	if ( !IsWindowVisible(hWnd) )
		return;

	ShowWindow( hWnd, SW_HIDE ); 
	SetProp(hWnd,DOHIDE_PROP,(bNeedRestore?HIDDEN_BY_DOHIDE_R:HIDDEN_BY_DOHIDE));
	HWND owner = GetWindow(hWnd,GW_OWNER);
	if ( owner ) 
		ShowWindow( owner, SW_HIDE ); 

	if ( !bHideToToolbar )
		return;

	TrayMinimizedList.Add( hWnd );

	if ( bPermanentTrayIcon )
		SetProp(hWnd,DOHIDE_PROP2,HIDDEN_BY_DOHIDE);

	NOTIFYICONDATA tnd;
	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = (UINT)hWnd;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_NOTIFYICON+1;
	tnd.hIcon            = hIcon;

	if ( tnd.hIcon==0 )
		tnd.hIcon = GetWindowIcon( hWnd );

	GetWndText( hWnd, tnd.szTip, sizeof(tnd.szTip) );
	ReplaceDesktopIcons( tnd.szTip );
	Shell_NotifyIcon( NIM_ADD, &tnd );
}


/*
 *
 *
 */
void ExitFromWindows( UINT flags )
{
	HANDLE hToken;              // handle to process token 
	TOKEN_PRIVILEGES tkp;       // pointer to token structure  
	
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, _SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);  
	tkp.PrivilegeCount = 1;  
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
	
	EnumWindows( (WNDENUMPROC)ShowHideAllWindowsProc, MAKELPARAM(HKID_SHOW_ALL,0) );
	ExitWindowsEx( flags,0 ); 
	
	tkp.Privileges[0].Attributes = 0; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);  
}


/*
 *
 *
 */
DWORD WINAPI RunScreenSaverThreadProc( PVOID lpParameter )
{
	Sleep(1000);
	PostMessage(hMainDlg, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
	return 0;
}
 


/*
 *
 *
 */
void HotKeyProc( WPARAM wParam )
{
	switch ( wParam ) 
	{
		 case HKID_SHOW_ALL:  
		 case HKID_HIDE_ALL:  	
		 case HKID_HIDE_ALL_TOOLBAR:  	
		 case HKID_MINIMIZE_ALL:  
		 case HKID_RESTORE_ALL:  
		 case HKID_CLOSE_ALL_WINDOWS:
		 case HKID_MAXIMIZE_ALL:
			EnumWindows( (WNDENUMPROC)ShowHideAllWindowsProc, MAKELPARAM(wParam,0) );

		_CASE HKID_HIDE_CURR: 
		 case HKID_HIDE_CURR_TOOLBAR:
		 case HKID_MINIMIZE_CURRENT:
		 case HKID_MAXIMIZE_CURRENT:
		 case HKID_SET_WINDOW_TOPMOST:
		 case HKID_MAXIMIZE_X:
		 case HKID_MAXIMIZE_Y:
		 case HKID_MOVE_TOP:
		 case HKID_MOVE_BOTTOM:
		 case HKID_MOVE_LEFT:
		 case HKID_MOVE_RIGHT:
			EnumWindows( (WNDENUMPROC)HideTopWindowProc, wParam );

		_CASE HKID_HIDE_AGAIN:
			bHideStill = true;

		_CASE HKID_RUN_SCREEN_SAVER:
			//PostMessage(hMainDlg, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
			{
			DWORD dwThreadId;
			CreateThread( 0, 0, RunScreenSaverThreadProc, 0, 0, &dwThreadId );
			}
		
		_CASE HKID_LOGOFF:
			ExitFromWindows( EWX_LOGOFF );

		_CASE HKID_SHUTDOWN:
			ExitFromWindows( EWX_SHUTDOWN | EWX_POWEROFF );

		_CASE HKID_REBOOT: 
			ExitFromWindows( EWX_REBOOT );

		_CASE HKID_HARD_SHUTDOWN:
			ExitFromWindows( EWX_SHUTDOWN | EWX_POWEROFF | EWX_FORCE );

		_CASE HKID_HARD_REBOOT:
			ExitFromWindows( EWX_REBOOT | EWX_FORCE );

		_CASE HKID_KILL_PROCESS :
			EnumWindows( (WNDENUMPROC)HideTopWindowProc, HKID_KILL_PROCESS );

		_CASE HKID_CONTROL_PANEL:
			SendMessage( hMainDlg, WM_NOTIFYICON_CONTROL, 0, WM_LBUTTONDOWN );

		_CASE HKID_DISPLAY_PROPERTIES:
			ShellExecute( 0, SHELL_OPEN, SHELL_CONTROL, DESK_CPL, "", SW_SHOWNORMAL);

		_CASE HKID_DESKTOP_SHORTCUTS:
			SendMessage( hMainDlg, WM_NOTIFYICON_DESKTOP, 0, WM_LBUTTONDOWN );
 
		_CASE HKID_DOCUMENTS:
			SendMessage( hMainDlg, WM_NOTIFYICON_RECENT, 0, WM_LBUTTONDOWN );

		_CASE HKID_OPEN_CD_TRAY:
		 case HKID_CLOSE_CD_TRAY:
			{
			    HMODULE hWinMM_DLL;

				if ( !(hWinMM_DLL = LoadLibrary(_winMM_dll)) )
		            return;

				typedef MCIERROR (*__mciSendString)(LPCTSTR,LPTSTR,UINT,HANDLE );

				FARPROC fp_mciSendString = GetProcAddress( hWinMM_DLL, _mciSendString );
				if ( fp_mciSendString )
				{
					if ( wParam==HKID_CLOSE_CD_TRAY )
						(*(__mciSendString)fp_mciSendString)(_CloseTray,0,0,0);
					else
						(*(__mciSendString)fp_mciSendString)(_OpenTray,0,0,0);
				}

				FreeLibrary( hWinMM_DLL );
			}

		_CASE HKID_FAVORITES:
			SendMessage( hMainDlg, WM_NOTIFYICON_FAVORITES, 0, WM_LBUTTONDOWN );

		_CASE HKID_DOHIDE_ICON: 
			TrayMessage( (bDoHideIconExist==false)?NIM_ADD:NIM_DELETE );	

		_CASE HKID_DIALUP_NETWORKING:
			SendMessage( hMainDlg, WM_NOTIFYICON_DIALUP, 0, WM_LBUTTONDOWN );
	}
}


/*
 *
 *
 */
void NotifyMessage ( LPARAM lParam )
{
	switch (lParam)
	{
		case WM_LBUTTONDOWN: 
		{
			if ( !CheckPassword(PWD_CONTEXT_MENU) )
				return;

			HMENU hMenu = LoadMenu(hResourceInst,MAKEINTRESOURCE(IDR_POPUPMENU));
			HMENU hSubMenu = GetSubMenu(hMenu,0);

			CustomMenu.Initialize(hSubMenu,-1);
			EnumWindows( (WNDENUMPROC)FillMenuWindowsProc, 0 );

			for ( int i=0; i<=MAX_USED_HOTKEY; i++ )
			{
				if ( !HotKeys.IsAddToMenu( HotKeys.Translate(i) ) )
					continue;

				char s[128];
				LoadStr( IDS_HOTKEY1+i,s,sizeof(s));
				char* s1 = strstr(s," ");
				if (s1==0) s1=s;
				else s1++;
				AppendMenu(hSubMenu, MF_BYPOSITION|MF_STRING, WM_HOTKEY_COMMAND+HotKeys.Translate(i), s1);
			}

			ShowMenu ( hMenu );
		}

		_CASE WM_RBUTTONDOWN: 
		{
			if ( !CheckPassword(PWD_CONTEXT_MENU) )
				return;

			HMENU hMenu = LoadMenu(hResourceInst,MAKEINTRESOURCE(IDR_POPUPMENU1));
			SetMenuDefaultItem( GetSubMenu(hMenu,0), IDC_CONFIG, FALSE );
			ShowMenu ( hMenu );
		}
/*
		_CASE WM_LBUTTONDBLCLK: 
		 case WM_RBUTTONDBLCLK: 
			CreateOptionsDialog();
*/
		_CASE WM_MOUSEMOVE:
			StateChange();
	}
}


/*
 *
 *
 */
void CommandMessage (WPARAM wParam, LPARAM lParam)
{
	int ID = GET_WM_COMMAND_ID(wParam, lParam);

	if ( ID>=WM_EXECUTE_CUSTOM_MENU && ID<WM_EXECUTE_CUSTOM_MENU+1000 )
	{
		CustomMenu.OnExecute( ID-WM_EXECUTE_CUSTOM_MENU );
	}
	else
	if ( ID>=WM_HOTKEY_COMMAND && ID<WM_HOTKEY_COMMAND+COUNT_HOTKEY )
	{
		HotKeyProc(ID-WM_HOTKEY_COMMAND);
	}
	else
	switch (ID)
	{
		 case IDC_CONFIG: 
			CreateOptionsDialog();

		_CASE ID_ABOUT: 
			{
			char HELPFILE_NAME[MAX_PATH];
			WinHelp( hMainDlg, GetHelpFileName(HELPFILE_NAME), HELP_FINDER, 0 ); 
			}

		_CASE IDCANCEL: 
			if ( !CheckPassword( PWD_CLOSE ) )
				break;

		 case ID_CLOSE_DOHIDE: 
			EnumWindows( (WNDENUMPROC)ShowHideAllWindowsProc, MAKELPARAM(HKID_SHOW_ALL,1) );
			ProgramList.RemoveAllIcon();
			if ( !uKey[12] )
				TrayMessage(NIM_DELETE);	
			if ( !uKey[10] )
				DesktopTrayMessage(NIM_DELETE);
			if ( !uKey[11] )
				RecentTrayMessage(NIM_DELETE);
			if ( !uKey[14] )
				FavoritesTrayMessage(NIM_DELETE);
			if ( !uKey[16] )
				DialupTrayMessage(NIM_DELETE);
			if ( !uKey[18] )
				ControlPanelTrayMessage(NIM_DELETE);
			if ( !uKey[19] )
				PrintersTrayMessage(NIM_DELETE);
			if ( !uKey[20] )
				CpuTrayMessage(NIM_DELETE,0);

			HotKeys.UnregisterAllHotKey();
			EndDialog(hMainDlg, TRUE);     
			hMainDlg = 0;

		_CASE ID_REGISTER:
			CreateOptionsDialog(5);

		_CASE ID_RUN_PROGRAM:
		 case ID_EDIT_PROGRAM:
			ProgramList.Command( ID );
	}
}


/*
 *
 *
 */
void SetupTimer()
{
	KillTimer(hMainDlg,1);
	SetTimer(hMainDlg,1,uKey[6],(TIMERPROC)&TimerCallbackProc);
}


/*
 *
 *
 */
void ShellMenu ( int iShellMenuType )
{
	POINT point;
	GetCursorPos(&point);
	HMENU hMenu = LoadMenu(hResourceInst,MAKEINTRESOURCE(IDR_POPUPMENU));
	HMENU hSubMenu = GetSubMenu(hMenu,0);
	DeleteMenu( hSubMenu, 2, MF_BYPOSITION );

	CustomMenu.Initialize( hSubMenu, iShellMenuType );

	if ( iShellMenuType==CSIDL_DIALUP )
	{
		ShellLink.ShellFillRasConnectionMenu( hSubMenu, 0 );
	}
	else
	{
		ShellLink.ShellFillMenu( hSubMenu, iShellMenuType );
	}

	if ( iShellMenuType!=CSIDL_FAVORITES )
	{
		CustomMenu.Sort();
	}

	Register->UpdateDayCounter();

	ShowMenu ( hMenu );
}


/*
 *
 *
 */
void CheckExplorerCrash()
{
	static HWND hwndShell = (HWND)0xFFFFFFFF;

	if ( hwndShell==(HWND)0xFFFFFFFF )
	{
		hwndShell = FindWindow(TRAY_WND, 0);
	}

	HWND hwnd = FindWindow(TRAY_WND, 0);
	if ( hwnd && hwndShell!=hwnd ) 
	{
		hwndShell = hwnd;
		bRestart = true;
		SendMessage(hMainDlg,WM_COMMAND,IDCANCEL,0);
	}

	CHECK_REGISTRATION( 352, IGNORE_COUNTER, 30, ACT_SHOW_WELCOME_DIALOG );
}

void AddTrayIconsXP()
{
	if ( !uKey[20] )
		CpuTrayMessage(NIM_ADD,0);
	if ( !uKey[19] )
		PrintersTrayMessage(NIM_ADD);
	if ( !uKey[18] )
		ControlPanelTrayMessage(NIM_ADD);
	if ( !uKey[16] )
		DialupTrayMessage(NIM_ADD);
	if ( !uKey[14] )
		FavoritesTrayMessage(NIM_ADD);
	if ( !uKey[11] )
		RecentTrayMessage(NIM_ADD);
	if ( !uKey[10] )
		DesktopTrayMessage(NIM_ADD);
	if ( !uKey[12] )
	{
		TrayMessage(NIM_ADD);
		StateChange();
	}
}

void AddTrayIcons()
{
	if ( !uKey[12] )
	{
		TrayMessage(NIM_ADD);
		StateChange();
	}
	if ( !uKey[10] )
		DesktopTrayMessage(NIM_ADD);
	if ( !uKey[11] )
		RecentTrayMessage(NIM_ADD);
	if ( !uKey[14] )
		FavoritesTrayMessage(NIM_ADD);
	if ( !uKey[16] )
		DialupTrayMessage(NIM_ADD);
	if ( !uKey[18] )
		ControlPanelTrayMessage(NIM_ADD);
	if ( !uKey[19] )
		PrintersTrayMessage(NIM_ADD);
	if ( !uKey[20] )
		CpuTrayMessage(NIM_ADD,0);
}

//
//
//
BOOL CALLBACK TestAppDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CheckExplorerCrash();

	CHECK_REGISTRATION( 4346, CLEAR_COUNTER, 35, ACT_SHOW_WELCOME_DIALOG );

	switch ( uMsg )
	{
		case WM_INITDIALOG:
			hMainDlg = hDlg;
			if ( isWinXP )
				AddTrayIconsXP();
			else
				AddTrayIcons();

			HotKeys.LoadFromRegistry();
			HotKeys.RegisterAllHotKey();
			SetupTimer();
			CreateSupportWindow();

		_CASE WM_DESTROY:
			KillTimer(hDlg,1);
			DestroySupportWindow();
		
		_CASE WM_ENDSESSION:
			if ( uKey[17]&PWD_CLOSE )
				CommandMessage (ID_CLOSE_DOHIDE,0);

		_CASE WM_HOTKEY:
			HotKeyProc(wParam);

		_CASE WM_COMMAND:
			CommandMessage (wParam, lParam);

		_CASE WM_COPYDATA:
			{
				WND_INFO* wi = (WND_INFO*)((COPYDATASTRUCT*)lParam)->lpData;
				switch ( wi->m_Command )
				{
					 case CM_PID:      
						ProcessList.AddProcess( wi );
					_CASE CM_ACTIVATE: 
						ShowOneWindow(wi->m_hWnd, false);
				}
			}

		_CASE WM_NOTIFYICON:
			NotifyMessage ( lParam );

		_CASE WM_NOTIFYICON+1: 
			switch (lParam)
			{
				case WM_LBUTTONDOWN: 
				case WM_LBUTTONDBLCLK: 
					if ( !IsWindowVisible((HWND)wParam) )
						ShowOneWindow( (HWND)wParam, true );
					else
						HideOneWindow( (HWND)wParam, true );
				_CASE WM_RBUTTONDOWN: 
					NotifyMessage ( lParam );
				_CASE WM_MOUSEMOVE:
					{
					NOTIFYICONDATA tnd;
					tnd.cbSize = sizeof(NOTIFYICONDATA);
					tnd.hWnd   = hMainDlg;
					tnd.uID	   = wParam;
					tnd.uFlags = NIF_TIP;
					GetWndText( (HWND)wParam, tnd.szTip, sizeof(tnd.szTip) );
					ReplaceDesktopIcons( tnd.szTip );
					Shell_NotifyIcon( NIM_MODIFY, &tnd );
					}
			}

		_CASE WM_NOTIFYICON_DESKTOP: 
			if ( lParam==WM_RBUTTONDOWN ) 
				NotifyMessage ( lParam );
			if ( lParam==WM_LBUTTONDOWN ) 
			{
				if ( !CheckPassword( PWD_CONTEXT_MENU ) )
					return 1;
				ShellMenu (CSIDL_DESKTOP);
			}

		_CASE WM_NOTIFYICON_RECENT: 
			if ( lParam==WM_RBUTTONDOWN ) 
				NotifyMessage ( lParam );
			if ( lParam==WM_LBUTTONDOWN ) 
			{
				if ( !CheckPassword( PWD_CONTEXT_MENU ) )
					return 1;
				ShellMenu (CSIDL_RECENT);
			}
		 
		_CASE WM_NOTIFYICON_FAVORITES: 
			if ( lParam==WM_RBUTTONDOWN ) 
				NotifyMessage ( lParam );
			if ( lParam==WM_LBUTTONDOWN ) 
			{
				if ( !CheckPassword( PWD_CONTEXT_MENU ) )
					return 1;
				ShellMenu (CSIDL_FAVORITES);
			}
		
		_CASE WM_NOTIFYICON_DIALUP: 
			if ( lParam==WM_RBUTTONDOWN ) 
				NotifyMessage ( lParam );
			if ( lParam==WM_LBUTTONDOWN ) 
			{
				if ( !CheckPassword( PWD_CONTEXT_MENU ) )
					return 1;
				ShellMenu (CSIDL_DIALUP);
			}

		_CASE WM_NOTIFYICON_CONTROL:
			if ( lParam==WM_RBUTTONDOWN ) 
				NotifyMessage ( lParam );
			if ( lParam==WM_LBUTTONDOWN ) 
			{
				if ( !CheckPassword( PWD_CONTEXT_MENU ) )
					return 1;
				ShellMenu (CSIDL_CONTROLS);
			}

		_CASE WM_NOTIFYICON_PRINTERS:
			if ( lParam==WM_RBUTTONDOWN ) 
				NotifyMessage ( lParam );
			if ( lParam==WM_LBUTTONDOWN ) 
			{
				if ( !CheckPassword( PWD_CONTEXT_MENU ) )
					return 1;
				ShellMenu (CSIDL_PRINTERS);
			}

		_CASE WM_TRAY_PROGRAM_NOTIFY:
			switch (lParam)
			{
				case WM_LBUTTONDOWN: 
					ProgramList.LMouseButtonDown((TProgramInfo*)wParam);
				_CASE WM_RBUTTONDOWN: 
					ProgramList.RMouseButtonDown((TProgramInfo*)wParam);
			}

		_CASE WM_NOTIFYICON_CPU: 
			if ( lParam==WM_RBUTTONDOWN ) 
				NotifyMessage ( lParam );
			if ( lParam==WM_LBUTTONDOWN ) 
			{
				if ( !CheckPassword( PWD_CONTEXT_MENU ) )
					return 1;
			}
		 
		_CASE WM_UNREGISTRED:
			CreateOptionsDialog( 0 );

		_CASE WM_CHECK_REGISTRATION:
			if ( IS_REGISTRED() )
			{
				Beep(1000,200);	
				Beep(500,200);
				Beep(1000,200);
			}

		_DEFAULT:
			return FALSE;
	}

	return TRUE;
}

/*
 *
 *
 */
void TrimLeft( char** s )
{
	while ( *(*s) && (*(*s))==' ' ) (*s)++;
}

/*
 *
 *
 */
void ProcessCmdLine ()
{
	LPTSTR s = GetCommandLine();

	int bQuotaMark = 0;
	while ( (*s) && ( (*s)!=' ' || bQuotaMark) )
	{
		if ( (*s)=='"' ) bQuotaMark ^= 1;
		s++;
	}
	if ( !(*s) ) return;

	ProcessDDEAndCmdLine(s);
}


/*
 *
 *
 */
void ProcessDDEAndCmdLine ( char* s )
{
	TrimLeft( &s );
	if ( !(*s) ) return;

	THideByRunInfo info;
	if ( s[0]=='/' )
	{
		char* pSpace = strstr(s," ");
		if ( pSpace )
		{
			*pSpace = 0;	
			info.StringToOptions( &(s[1]) );
			s = pSpace+1;
			TrimLeft( &s );
			if ( !(*s) ) return;
		}
	}
	lstrcpy(info.m_szTitle[0],s);

	THideWindow hide(&info,false);
	EnumWindows( (WNDENUMPROC)HideWindowsProc, (long)&hide );

	if ( strstr(s,PRM_OPTIONS1) || strstr(s,PRM_OPTIONS2) )
	{
		PostMessage(hMainDlg,WM_COMMAND,IDC_CONFIG,0);
	}
	
	if ( s[0]=='/' && s[1]=='x' && s[2]==0 ) 
	{
		PostMessage( hMainDlg, WM_CHECK_REGISTRATION, 0, 0 );
	}
}


//
//---------------------------------------------------------------------------
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;

	HANDLE hDoHideMutex = OpenMutex( MUTEX_ALL_ACCESS, FALSE, DOHIDE );  
	if ( hDoHideMutex )
	{
		hMainDlg = FindWindow(0,DOHIDE);
		ProcessCmdLine();
		return 0;
	}
	hDoHideMutex = CreateMutex( 0, FALSE, DOHIDE );

	lib = new CLibrary;
	Security = new CSecurity;

	DDE.Init();
	InitCommonControls();      
	LoadConfiguration();

	Register = new TRegistration;
	
	if ( !LoadLanguageDll( uKey[0] ) )
	{
		if ( !LoadLanguageDll( 0 ) )
		{
			char s1[128];
			char s2[128];

			LoadString( hInst, IDS_UNABLE_LOAD_LANGUAGE_DLL, s1, sizeof(s1) );
			LoadString( hInst, IDS_DOHIDE_TITLE, s2, sizeof(s2) );
			MessageBeep( 0xFFFFFFFF );
			MessageBox( 0, s1, s2, MB_OK|MB_ICONSTOP );

			delete Register;
			delete Security;
			delete lib;

			ReleaseMutex( hDoHideMutex );
			CloseHandle( hDoHideMutex );

			return 1;
		}

		uKey[0] = 0;
	}

	uKey[4] &= 0xFFFFFF00;
	uKey[4] |= DOHIDE_VERSION;
	SetHook(1);

	CreateDialog( hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)TestAppDlgProc );

	ProcessCmdLine();
	ProgramList.UpdateList();

	RegisterAsService( 1 );

	MSG msg;
	while ( GetMessage(&msg, 0, 0, 0) && hMainDlg!=0 ) 
	{
		if ( !IsWindow(hMainDlg) || !IsDialogMessage(hMainDlg, &msg) ) 
		{ 
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		} 

		if ( msg.time-dwAppCurrTime>1000 )
		{
			SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
		}

#ifdef NEW_DEFENCE
		if ( dwAppStartTime==0 )			
			dwAppStartTime = msg.time;
		dwAppCurrTime = msg.time;
#endif
	} 

	RegisterAsService( 0 );

	SetHook(0);
	StoreConfiguration();

	delete Register;
	delete Security;
	delete lib;

	FreeLibrary( hResourceInst );

	ReleaseMutex( hDoHideMutex );
	CloseHandle( hDoHideMutex );

	if ( bRestart )
	{
		char fName[MAX_PATH];
		GetModuleFileName( GetModuleHandle(0), fName, sizeof(fName) );
		ShellExecute(0, SHELL_OPEN, fName, 0, "", SW_HIDE );
	}

	return 0;
} 