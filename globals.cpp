#include "stdafx.h"

#include "DoHide.h"
#include "register.h"
#include "globals.h"
#include "WinList.h"
#include "menu.h"

#include "resource.h"


#ifdef OEM_VERSION
	#ifdef OEM_NAME_E_AND_S
		const char DOHIDE_VER[]		  = "DoHide v3.3 (E&S) - "; 
	#endif
#else
const char DOHIDE_VER[]				  = "DoHide v3.3 - "; 
#endif

const char CURRENT_DATE[]			  = __DATE__;
const char STAR[]					  = "*";
const char KERNEL_DLL[]				  = "kernel32.dll";
const char REGISTER_SERVICE_PROCESS[] = "RegisterServiceProcess";
const char SUPPORT_WINDOW_CLASS_NAME[]= "DoHideSupportWndClass";

extern bool GetVersionInformation();

bool isWinNT   = GetVersionInformation();
bool isWinNT2K = false;
bool isWinXP = false;

HWND hSupportWindow = 0;

/*
 *
 *
 */
bool GetVersionInformation()
{
	isWinNT2K = false;
	isWinXP = false;

	OSVERSIONINFO osvi;
	
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	
	if ( osvi.dwPlatformId!=VER_PLATFORM_WIN32_NT )
	{
		return false;
	}
	else
	{
		if ( osvi.dwMajorVersion>=5 )
		{
			if ( osvi.dwMinorVersion==0 )
				isWinNT2K = true;
			else if ( osvi.dwMinorVersion>=1 )
				isWinXP = true;
		}
	}
	return true;
}

/*
 *
 *
 */
void InitFileNameStruct ( OPENFILENAME& ofn )
{
	ofn.lStructSize       = sizeof( OPENFILENAME );
	ofn.hInstance         = 0;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = 0;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = 0;  // Title for dialog
	ofn.Flags             = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NODEREFERENCELINKS;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = STAR;
	ofn.lCustData         = NULL;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
} 


/*
 *
 *
 */
HICON GetFileIcon( char* szIconFile, int iIcon )
{
	if ( szIconFile==0 || szIconFile[0]==0 )
		return 0;

	SHFILEINFO shfi;
	HICON hIcon = ExtractIcon(hInst,szIconFile,iIcon);

	if ( (int)hIcon==0 || (int)hIcon==1 )
	{
		if (SHGetFileInfo ((LPCSTR)szIconFile, 0, &shfi, sizeof (SHFILEINFO), SHGFI_ICON ))
		{
			return shfi.hIcon;
		}
	}

	return hIcon;
}

/*
 *
 *
 */
HICON GetWindowIcon( HWND hWnd )
{
	HICON hIcon = (HICON)::SendMessage(hWnd, WM_QUERYDRAGICON, 0, 0);

	if ( hIcon==0 )
	{
		WND_INFO* WndInfo = ProcessList.FindWindowInfo( hWnd );
		if ( WndInfo )
		{
			hIcon = GetFileIcon( WndInfo->m_szModuleName, 0 );
		}
		if ( hIcon==0 )
		{
			hIcon = (HICON)GetClassLong( hWnd, GCL_HICONSM );

			if ( hIcon==0 )
			{
				hIcon = (HICON)GetClassLong( hWnd, GCL_HICON );
			
				if ( hIcon==0 )
				{
					hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON2));
				}
			}
		}
	}

	return hIcon;
}


/*
 *
 *
 */
char* LoadStr( int ID, char* s, int size )
{
	LoadString(hResourceInst,ID,s,size);
	return s;
}


/*
 *
 *
 */
void FreeMenu ( HMENU hMenu )
{
	int count = GetMenuItemCount(hMenu);
	for ( int i=0; i<count; i++ )
	{
		HMENU hSubMenu = GetSubMenu(hMenu,i);
		if ( hSubMenu )
			FreeMenu(hSubMenu);
	}

	DestroyMenu(hMenu);

	CHECK_REGISTRATION_2( 3, IGNORE_COUNTER, 60, ACT_KILL_DOHIDE );
}


/*
 *
 *
 */
void ShowMenu ( HMENU hMenu )
{
	POINT point;
	GetCursorPos(&point);
	SetForegroundWindow(hMainDlg);
	
	PRINTF("Begin TrackPopupMenuEx");
	TrackPopupMenuEx(GetSubMenu(hMenu,0),TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,hSupportWindow,0);
//	TrackPopupMenuEx(GetSubMenu(hMenu,0),TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,hMainDlg,0);
	PRINTF("End TrackPopupMenuEx");
	
	PostMessage(hMainDlg, WM_NULL, 0, 0);
	FreeMenu( hMenu );
}


/*
 *
 *
 */
void WinHelp( int ID )
{
	char HELPFILE_NAME[MAX_PATH];
	WinHelp( hMainDlg, GetHelpFileName(HELPFILE_NAME), HELP_CONTEXT, ID ); 
}



/*
 *
 *
 */
void GetWndText( HWND hWnd, char* s, int str_len )
{
	GetWindowText( hWnd, s, str_len );
/*	
	DWORD dwResult;
	s[0] = 0;

	SendMessageTimeout( hWnd, WM_GETTEXT, (WPARAM)str_len, (LPARAM)s, SMTO_NORMAL, 10, &dwResult );
*/
}


/*
 *
 *
 */
int GetWndTextLen( HWND hWnd )
{
	return GetWindowTextLength( hWnd );
/*
	DWORD dwResult = 0;

	SendMessageTimeout( hWnd, WM_GETTEXTLENGTH, 0, 0, SMTO_NORMAL, 10, &dwResult );

	return dwResult;
*/
}


/* 
 *
 *
 */
bool CompareTitle ( char* szTitle1, char* szTitle2, bool bCaseInsensitive, bool bFullTitle )
{
	if ( bCaseInsensitive ) 
	{
		if ( bFullTitle) 
		{
			return (lstrcmpi(szTitle1,szTitle2)==0);
		}
		else
		{
			char tmp1[MAX_PATH];
			char tmp2[MAX_PATH];
			lstrcpy(tmp1,szTitle1);
			lstrcpy(tmp2,szTitle2);
			CharUpper(tmp1);
			CharUpper(tmp2);
			
			return (strstr(tmp1,tmp2)!=0);
		}
	}
	else
	{
		return ( bFullTitle )? (lstrcmp(szTitle1,szTitle2)==0):(strstr(szTitle1,szTitle2)!=0);
	}
}


/*
 *
 *
 */
bool CompareTitle ( THideByRunInfo* info, char* buf )
{
	bool bFindTitle = false;

	int cnt = info->GetTitleCount();

	for ( int i=0; i<=cnt && !bFindTitle; i++ )
	{
		bFindTitle = CompareTitle( buf, info->GetTitle(i), info->m_bHideCaseInsensitive, info->m_bHideByFullTitle );
	}

	return bFindTitle;
}



/*
 *
 *
 */
void CreateOptionsDialogTitle( char* s )
{
	lstrcpy( s, DOHIDE_VER   );
    lstrcat( s, CURRENT_DATE );
}


/*
 *
 *
 */
char* CvtString( char* s )
{
	int len = lstrlen(s);
	
	for ( int i=0; i<len; i++ )
		if ( s[i]=='~' )
			s[i]=0;

	return s;
}


/*
 *
 *
 */
void RegisterAsService( int iRegister )
{
	if ( isWinNT )
		return;

	HINSTANCE hKernel = LoadLibrary( KERNEL_DLL );
	if ( !hKernel )
		return;
  
	typedef DWORD (*__RegisterServiceProcess)( DWORD, DWORD );

	__RegisterServiceProcess lpRegisterServiceProcess = (__RegisterServiceProcess)GetProcAddress( hKernel, REGISTER_SERVICE_PROCESS );
	if ( lpRegisterServiceProcess )
	{
		(*lpRegisterServiceProcess)( 0, iRegister );
	}

	FreeLibrary(hKernel);
}


/*
 *
 *
 */
char* GetHelpFileName ( char* s )
{
	char s1[MAX_PATH];

	GetModuleFileName( hInst, s, MAX_PATH );
	s[lstrlen(s)-10] = 0;
	LoadStr( IDS_LANGUAGE_HELP, s1, sizeof(s1) );
	lstrcat( s, s1 );
	
	return s;
}


/*
 *
 *
 */
bool IsWindowVisibleOnScreen ( HWND hWnd )
{
	RECT R;
	R.left   = R.top = 0;
	R.right  = GetSystemMetrics(SM_CXSCREEN);
	R.bottom = GetSystemMetrics(SM_CYSCREEN);

	RECT r;
	GetWindowRect(hWnd,&r);

	POINT p;

	p.x = r.left;
	p.y = r.top;
	if ( PtInRect(&R,p) )
		return true;
	
	p.x = r.left;
	p.y = r.bottom;
	if ( PtInRect(&R,p) )
		return true;
	
	p.x = r.right;
	p.y = r.top;
	if ( PtInRect(&R,p) )
		return true;
	
	p.x = r.right;
	p.y = r.bottom;
	if ( PtInRect(&R,p) )
		return true;

	return false;
}











/*
 *
 *
 */
LRESULT CALLBACK SupportWindowProc(	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case WM_MEASUREITEM: 
			CustomMenu.OnMeasureItem(hWnd, (LPMEASUREITEMSTRUCT) lParam); 
 
        _CASE WM_DRAWITEM: 
			CustomMenu.OnDrawItem((LPDRAWITEMSTRUCT) lParam); 

		_CASE WM_MENUCHAR:
			return CustomMenu.OnMenuChar((char)(LOWORD(wParam)), (HMENU)lParam); 

		_CASE WM_COMMAND:
			CommandMessage (wParam, lParam);
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
 

/*
 *
 *
 */
void CreateSupportWindow()
{
	WNDCLASS wc; 

    wc.style = 0; 
    wc.lpfnWndProc = &SupportWindowProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = hInst; 
    wc.hIcon = 0; 
    wc.hCursor = 0; 
    wc.hbrBackground = 0; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = SUPPORT_WINDOW_CLASS_NAME; 
 
	RegisterClass( &wc );

	hSupportWindow = CreateWindow(
		SUPPORT_WINDOW_CLASS_NAME,  // pointer to registered class name
		"",		   // pointer to window name
		WS_CHILD|WS_VISIBLE,  // window style
		-1000,     // horizontal position of window
		-1000,     // vertical position of window
		999,       // window width
		999,       // window height
		hMainDlg,  // handle to parent or owner window
		0,         // handle to menu or child-window identifier
		hInst,     // handle to application instance
		0		   // pointer to window-creation data
	);
}


/*
 *
 *
 */
void DestroySupportWindow()
{
	DestroyWindow( hSupportWindow );
	UnregisterClass(SUPPORT_WINDOW_CLASS_NAME,hInst);
}