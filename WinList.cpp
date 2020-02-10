#include "stdafx.h"

#include "DoHide.h"
#include "bitmap.h"
#include "Globals.h"
#include "registry.h"
#include "WinList.h"
#include "Options.h"
#include "shell.h"

#include "resource.h"

const char SECTION_APP[]		= "App%02d";
const char DIVIDER[]			= "|";
const char DOT					= '.';

const char SECTION_HIDE[]		= "HideByRun\\";
const char ITEM_TITLE[]			= "Title";
const char ITEM_OPTIONS[]		= "Options";
const char ITEM_ICONFILE[]		= "IconFile";
const char ITEM_ICONID[]		= "IconID";
const char ITEM_BUTTONTITLE[]	= "ButtonTitle";

const char SECTION_PROG[]		= "TrayPrograms\\";
const char ITEM_PATH[]			= "Path";
const char ITEM_CMDLINE[]		= "CmdLine";
const char ITEM_STARTDIR[]		= "StartDir";
const char ITEM_ICON[]			= "Icon";
const char ITEM_RUNSTART[]		= "RunOnStartup";
const char ITEM_NOICONMARK[]	= "NoIconMark";
const char ITEM_DISABLED[]		= "Disabled";

const char CHAR_T[] = "T";
const char CHAR_M[] = "M";
const char CHAR_A[] = "A";
const char CHAR_N[] = "N";
const char CHAR_F[] = "F";
const char CHAR_E[] = "E";
const char CHAR_K[] = "K";
const char CHAR_S[] = "S";
const char CHAR_C[] = "C";
const char CHAR_P[] = "P";
const char CHAR_H[] = "H";
const char CHAR_O[] = "O";
const char CHAR_W[] = "W";
const char CHAR_B[] = "B";
const char CHAR_I[] = "I";
const char CHAR_D[] = "D";
const char CHAR_R[] = "R";
const char CHAR_G[] = "G";
const char CHAR_Z[] = "Z";


TProcessList			ProcessList;
THideByRunList			HideTitleList;
TProgramList			ProgramList;
TTrayMinimizedWindows	TrayMinimizedList;


//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////
THideByRunInfo::THideByRunInfo() 
{
	ZeroMemory(	this, sizeof(THideByRunInfo) );
/*
 *	Removed in version 3.1
 *
 *
	m_bHideFirstTime = true;
*/
}

char* THideByRunInfo::OptionsToString( char* s )
{
	if ( Empty() ) s[0] = 0;
	else 
	{
		s[0] = m_bHideToToolbar?'T':DOT;
		s[1] = m_bHideIfMinimized?'M':DOT;
		s[2] = m_bHideAlwaysIfMinimized?'A':DOT;
		s[3] = m_bHideAndIfMinimized?'N':DOT;
		s[4] = m_bHideByFullTitle?'F':DOT;
		s[5] = m_bCloseWindow?'E':DOT;
		s[6] = m_bKillProcess?'K':DOT;
		s[7] = m_bBeep?'S':DOT;
		s[8] = m_bHideCaseInsensitive?'C':DOT;
		s[9] = m_bFilePath?'P':DOT;
		s[10]= m_bShowWindow?'H':DOT;
		s[11]= m_bMoveToOutside?'O':DOT;
		s[12]= m_bWindowClass?'W':DOT;
		s[13]= m_bPressButton?'B':DOT;
		s[14]= m_bPermanentIcon?'I':DOT;
		s[15]= m_bDeleteTrayIcon?'R':DOT;
		s[16]= m_bHideFirstTime?'G':DOT;
		s[17]= m_bMaximizeWindow?'Z':DOT;
		s[18]= 0;
	}
	return s;
}

char* THideByRunInfo::OptionsToString2( char* s )
{
	if ( Empty() ) s[0] = 0;
	else 
	{
		s[18] = m_bDisabled?'D':DOT;
		s[19] = 0;
	}
	return s;
}

void THideByRunInfo::StringToOptions( char* s )
{
	CharUpper(s);
	m_bHideToToolbar         = strstr(s,CHAR_T)?true:false;
	m_bHideIfMinimized       = strstr(s,CHAR_M)?true:false;
	m_bHideAlwaysIfMinimized = strstr(s,CHAR_A)?true:false;
	m_bHideAndIfMinimized    = strstr(s,CHAR_N)?true:false;
	m_bHideByFullTitle       = strstr(s,CHAR_F)?true:false;
	m_bCloseWindow           = strstr(s,CHAR_E)?true:false;
	m_bKillProcess           = strstr(s,CHAR_K)?true:false;
	m_bBeep                  = strstr(s,CHAR_S)?true:false;
	m_bHideCaseInsensitive   = strstr(s,CHAR_C)?true:false;
	m_bFilePath              = strstr(s,CHAR_P)?true:false;
	m_bShowWindow            = strstr(s,CHAR_H)?true:false;
	m_bMoveToOutside         = strstr(s,CHAR_O)?true:false;
	m_bWindowClass	         = strstr(s,CHAR_W)?true:false;
	m_bPressButton	         = strstr(s,CHAR_B)?true:false;
	m_bPermanentIcon         = strstr(s,CHAR_I)?true:false;
	m_bDisabled              = strstr(s,CHAR_D)?true:false;
	m_bDeleteTrayIcon        = strstr(s,CHAR_R)?true:false;
	m_bHideFirstTime         = strstr(s,CHAR_G)?true:false;
	m_bMaximizeWindow		 = strstr(s,CHAR_Z)?true:false;
}


void THideByRunInfo::StoreToRegistry( int i )
{
	char s1[128];
	char folder[128];

	wsprintf( s1, SECTION_APP, i );

	lstrcpy(folder,SECTION_DOHIDE); 
	lstrcat(folder,SECTION_HIDE); 
	lstrcat(folder,s1);
	OptionsToString ( s1 );
	OptionsToString2( s1 );

	char szTitles[1024];
	UniteTitles( szTitles );
	WriteKey( HKEY_CURRENT_USER, folder, ITEM_TITLE,		szTitles );
	WriteKey( HKEY_CURRENT_USER, folder, ITEM_OPTIONS,		s1 );
	WriteKey( HKEY_CURRENT_USER, folder, ITEM_ICONFILE,		m_szIconFile );
	WriteKey( HKEY_CURRENT_USER, folder, ITEM_ICONID,		m_iIcon );
	WriteKey( HKEY_CURRENT_USER, folder, ITEM_BUTTONTITLE,  m_szPressButtonTitle );
}

void THideByRunInfo::LoadFromRegistry( int i )
{
	char s[20];
	char s1[128];
	char s2[128];

	wsprintf( s1, SECTION_APP, i );
	lstrcpy(s2,SECTION_DOHIDE);
	lstrcat(s2,SECTION_HIDE);
	lstrcat(s2,s1);

	char szTitle[255];
	szTitle[0] = 0;
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_TITLE, (char**)&szTitle, sizeof(szTitle) );
	SplitTitles( szTitle );

	if ( Empty() ) 
		return;

	ReadKey( HKEY_CURRENT_USER, s2, ITEM_OPTIONS,     (char**)&s, sizeof(s) );
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_ICONFILE,    (char**)&m_szIconFile, sizeof(m_szIconFile) );
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_ICONID,	  (char**)&m_iIcon, sizeof(m_iIcon) );
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_BUTTONTITLE, (char**)&m_szPressButtonTitle, sizeof(m_szPressButtonTitle) );

	StringToOptions(s);
}


HICON THideByRunInfo::GetFileIcon( )
{
	return ::GetFileIcon( m_szIconFile, m_iIcon );
}


void THideByRunInfo::SplitTitles( char* szTitle )
{
	m_iTitleCount = 0;
	int idx = 0;
	char* ptr = &szTitle[0];
	do
	{
		if ( (*ptr)=='|' ) 
		{
			m_szTitle[m_iTitleCount][idx] = 0;
			idx = 0;
			m_iTitleCount++;
		}
		else 
		{
			m_szTitle[m_iTitleCount][idx] = (*ptr);
			idx++;
		}

		ptr++;
	} 
	while( (*ptr) && m_iTitleCount<5 );

	m_szTitle[m_iTitleCount][idx] = 0;
}

char* THideByRunInfo::UniteTitles( char* szTitle )
{
	szTitle[0] = 0;
	for ( int j=0; j<=GetTitleCount(); j++ )
	{
		lstrcat(szTitle,GetTitle(j));
		lstrcat(szTitle,DIVIDER);
	}

	szTitle[lstrlen(szTitle)-1] = 0;
	return szTitle;
}


//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////
TProgramInfo::TProgramInfo() 
{
	ZeroMemory(	this, sizeof(TProgramInfo) );
}

void TProgramInfo::StoreToRegistry( int i )
{
	char s1[128];
	char s2[128];

	wsprintf( s1, SECTION_APP, i );

	lstrcpy(s2,SECTION_DOHIDE); lstrcat(s2,SECTION_PROG); lstrcat(s2,s1);
	WriteKey( HKEY_CURRENT_USER, s2, ITEM_TITLE,      m_szDescription );
	WriteKey( HKEY_CURRENT_USER, s2, ITEM_PATH,       m_szFilePath );
	WriteKey( HKEY_CURRENT_USER, s2, ITEM_CMDLINE,    m_szCommandLine );
	WriteKey( HKEY_CURRENT_USER, s2, ITEM_STARTDIR,   m_szStartDir );
	WriteKey( HKEY_CURRENT_USER, s2, ITEM_ICON,       m_iIcon );
	WriteKey( HKEY_CURRENT_USER, s2, ITEM_RUNSTART,   m_iRunOnStartup );
	WriteKey( HKEY_CURRENT_USER, s2, ITEM_NOICONMARK, m_iNoIconMark );

	if ( m_iRunOnStartup )
	{
		CreateStartupLink( m_szFilePath, m_szDescription, m_szCommandLine, m_szStartDir );
	}
	else
	{
		DeleteStartupLink( m_szDescription );
	}
	DeleteKey(HKEY_LOCAL_MACHINE, _REGSTR_PATH_RUN, m_szDescription );
}

void TProgramInfo::LoadFromRegistry( int i )
{
	char  s1[128];
	char  s2[128];

	wsprintf( s1, SECTION_APP, i );
	lstrcpy(s2,SECTION_DOHIDE);
	lstrcat(s2,SECTION_PROG);
	lstrcat(s2,s1);

	ReadKey( HKEY_CURRENT_USER, s2, ITEM_TITLE, (char**)&m_szDescription, sizeof(m_szDescription) );

	if ( Empty() ) 
		return;

	ReadKey( HKEY_CURRENT_USER, s2, ITEM_PATH,       (char**)&m_szFilePath,    sizeof(m_szFilePath) );
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_CMDLINE,    (char**)&m_szCommandLine, sizeof(m_szCommandLine) );
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_STARTDIR,   (char**)&m_szStartDir,    sizeof(m_szStartDir) );
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_ICON,       (UINT&)m_iIcon,           0 );
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_RUNSTART,   (UINT&)m_iRunOnStartup,   0 );
	ReadKey( HKEY_CURRENT_USER, s2, ITEM_NOICONMARK, (UINT&)m_iNoIconMark,     0 );
}

HICON TProgramInfo::GetFileIcon( )
{
	return ::GetFileIcon( m_szFilePath, m_iIcon );
}


char* TProgramInfo::GetTitleForTray( char* s )
{
	LoadStr(IDS_RUN_FROM_TRAY,s,256);
	return lstrcat( s,m_szDescription );
}

//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////
TProcessList::TProcessList () 
{
	ZeroMemory(	&m_WndInfo, sizeof(m_WndInfo) );
}

void TProcessList::AddProcess( WND_INFO* wInfo )
{
	for ( int i=0; i<MAX_WINDOWS; i++ )
	{
		if ( GetProcessVersion( m_WndInfo[i].m_PID )==0 || m_WndInfo[i].m_szModuleName[0]==0 || 
			 (wInfo->m_PID==m_WndInfo[i].m_PID && lstrcmpi(wInfo->m_szModuleName,m_WndInfo[i].m_szModuleName) )
		   )
		{
			CopyMemory( &m_WndInfo[i], wInfo, sizeof(WND_INFO) );
			break;
		}
	}
}

WND_INFO* TProcessList::FindWindowInfo( HWND hWnd )
{
	DWORD dwProcessId;
	GetWindowThreadProcessId( hWnd, &dwProcessId ); 
 
	for ( int i=0; i<MAX_WINDOWS; i++ )
	{
		if ( m_WndInfo[i].m_PID==dwProcessId )
			return &m_WndInfo[i];
	}
	return 0;
}

bool TProcessList::WindowTitleExist( HWND hWnd, char* s )
{
	WND_INFO* wInfo = FindWindowInfo( hWnd );

	if ( wInfo )
	if ( !lstrcmpi(wInfo->m_szModuleName,s) )
		return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////

THideByRunList::THideByRunList () 
{
	ZeroMemory(	&m_HideInfo, sizeof(m_HideInfo) );
	UpdateList();
}


THideByRunList::~THideByRunList () 
{
	FreeList();
}


void THideByRunList::FreeList ()
{
	for ( int i=0; i<MAX_WINDOWS; i++ ) 
		delete m_HideInfo[i];
	ZeroMemory(	&m_HideInfo, sizeof(m_HideInfo) );
}


void THideByRunList::UpdateList () 
{
	FreeList();
	m_iCount = 0;
	m_iDeleteFromTrayCount = 0;

	for ( int i=0; i<MAX_WINDOWS; i++ )
	{	
		THideByRunInfo* info = new THideByRunInfo;
		info->LoadFromRegistry(i);
		if ( info->Empty() )
		{
			delete info;
			continue;
		}
		Add(info);
	}
}

void THideByRunList::StoreToRegistry( )
{
	THideByRunInfo DefaultInfo;

	for ( int i=0; i<MAX_WINDOWS; i++ )
	{	
		THideByRunInfo* info = m_HideInfo[i];
		if ( info==0 )
			info = &DefaultInfo; 
		info->StoreToRegistry(i);
	}
}


void THideByRunList::Delete( int i ) 
{ 
	delete m_HideInfo[i]; 
	memcpy( &m_HideInfo[i], &m_HideInfo[i+1], (MAX_WINDOWS-i-1)*sizeof(THideByRunInfo*) );
	m_HideInfo[MAX_WINDOWS-1] = 0;
	m_iCount--;
}


void THideByRunList::Add( THideByRunInfo* info ) 
{ 
	m_HideInfo[m_iCount++] = info;

	if ( info->m_bDeleteTrayIcon && !info->m_bDisabled )
		m_iDeleteFromTrayCount++;
}


//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////

TProgramList::TProgramList () 
{
	ZeroMemory(	&m_ProgInfo, sizeof(m_ProgInfo) );
}


TProgramList::~TProgramList () 
{
	FreeList ();
}


void TProgramList::FreeList ()
{
	for ( int i=0; i<MAX_WINDOWS; i++ ) 
		delete m_ProgInfo[i];
	ZeroMemory(	&m_ProgInfo, sizeof(m_ProgInfo) );
}


void TProgramList::UpdateList () 
{
	FreeList();
	m_iCount = 0;

	for ( int i=0; i<MAX_WINDOWS; i++ )
	{	
		TProgramInfo* info = new TProgramInfo;
		info->LoadFromRegistry(i);
		if ( info->Empty() )
		{
			delete info;
			continue;
		}
		Add(info);
	}
}

void TProgramList::StoreToRegistry( )
{
	TProgramInfo DefaultInfo;

	for ( int i=0; i<MAX_WINDOWS; i++ )
	{	
		TProgramInfo* info = m_ProgInfo[i];
		if ( info==0 )
			info = &DefaultInfo; 
		info->StoreToRegistry(i);
	}
}


void TProgramList::PTrayMessage(DWORD dwMessage, TProgramInfo* info)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hMainDlg;
	tnd.uID			     = (UINT)info;
	tnd.uFlags		     = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAY_PROGRAM_NOTIFY;
	tnd.hIcon		     = info->m_iNoIconMark ? info->GetFileIcon() : DottedIcon(info->GetFileIcon(),32,32);

	char s[256];
	info->GetTitleForTray(s);
	int len = lstrlen(s)+1;
	lstrcpyn(tnd.szTip, info->GetTitleForTray(s), (len>64)?64:len );

	Shell_NotifyIcon(dwMessage, &tnd);
}

void TProgramList::Delete( int i ) 
{ 
	if ( m_ProgInfo[i]==0 )
		return;
	DeleteStartupLink( m_ProgInfo[i]->m_szDescription );
	DeleteKey(HKEY_LOCAL_MACHINE, _REGSTR_PATH_RUN, m_ProgInfo[i]->m_szDescription );
	PTrayMessage(NIM_DELETE, m_ProgInfo[i]);
	delete m_ProgInfo[i]; 
	memcpy( &m_ProgInfo[i], &m_ProgInfo[i+1], (MAX_WINDOWS-i-1)*sizeof(TProgramInfo*) );
	m_ProgInfo[MAX_WINDOWS-1] = 0;
	m_iCount--;
}

void TProgramList::Add( TProgramInfo* info ) 
{ 
	m_ProgInfo[m_iCount++] = info;
	PTrayMessage(NIM_ADD, info);
}

void TProgramList::UpdateIcon( TProgramInfo* info )
{
	PTrayMessage(NIM_MODIFY, info);
}

void TProgramList::RemoveAllIcon()
{
	for ( int i=0; i<MAX_WINDOWS; i++ )
	{	
		if ( m_ProgInfo[i]!=0 )
			PTrayMessage(NIM_DELETE, m_ProgInfo[i]);
	}
}

void TProgramList::LMouseButtonDown(TProgramInfo* info)
{
	ShellExecute( 0, SHELL_OPEN, info->m_szFilePath, info->m_szCommandLine, info->m_szStartDir, SW_SHOWNORMAL);
}

void TProgramList::RMouseButtonDown(TProgramInfo* info)
{
	m_iLastProgramInfo = info;

	HMENU hMenu = LoadMenu(hResourceInst,MAKEINTRESOURCE(IDR_POPUPMENU2));
	SetMenuDefaultItem( GetSubMenu(hMenu,0), ID_RUN_PROGRAM, FALSE );
	ShowMenu ( hMenu );
}

void TProgramList::Command( int ID )
{
	if ( ID==ID_RUN_PROGRAM )
		LMouseButtonDown(m_iLastProgramInfo);
	else
	if ( ID==ID_EDIT_PROGRAM )
		CreateOptionsDialog ( 4 );
}


//////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////

TTrayMinimizedWindows::TTrayMinimizedWindows () 
{
	FillMemory(	&m_hWnd, sizeof(m_hWnd), -1 );
}

TTrayMinimizedWindows::~TTrayMinimizedWindows () 
{
}

void TTrayMinimizedWindows::Add( HWND hWnd ) 
{ 
	for ( int i=0; i<MAX_WINDOWS; i++ ) 
	{
		if ( m_hWnd[i]==(HWND)-1 )
		{
			m_hWnd[i] = hWnd;
			break;
		}
	}
}

void TTrayMinimizedWindows::CheckWindows()
{
	for ( int i=0; i<MAX_WINDOWS; i++ ) 
	{
		HWND hWnd = m_hWnd[i];
		if ( hWnd!=(HWND)-1 )
		{
			if ( !IsWindow(hWnd) || !IsIconic(hWnd) )
			{
				PRINTF("%d %d ", hWnd, GetProp(hWnd,DOHIDE_PROP2));
				//ShowOneWindow(hWnd,false,(GetProp(hWnd,DOHIDE_PROP2)?false:true));
				ShowOneWindow(hWnd,false,true);
				m_hWnd[i] = (HWND)-1;
			}
		}
	}			
}
