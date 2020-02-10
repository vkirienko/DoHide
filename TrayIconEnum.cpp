/*
 *	Tray icon enumerator
 *	
 *	Based on Nick N. Repin sources
 *
 */

//----------------------------------------------------------------------------
//
// You are allowed to distribute this document unmodified for free, 
// without paying me any fee. 
// The information in this document is provided AS IS, 
// without any warranties or guarantees.
//
// All undocumented Windows features used/described in this document
// are discovered by me, Nick N. Repin.
//
// Copyright (c) Nikolay N. Repin (Nick N. Repin), 1998
// http://skyscraper.fortunecity.com/gigo/311/winprog.html
//	
//----------------------------------------------------------------------------

#include "stdafx.h"

#include "TrayIconEnum.h"
#include "globals.h"
#include "winlist.h"
#include <commctrl.h>


const char TRAY_WND[] 		 = "Shell_TrayWnd";
const char TRAY_NOTIFY_WND[] = "TrayNotifyWnd";


struct TWIconData_NT 
{
   DWORD           internalId;  // image index, -1 if no image in iconList
   NOTIFYICONDATAW d;
};

struct TWIconData_95 
{
   DWORD           internalId;  // image index, -1 if no image in iconList
   NOTIFYICONDATAA d;
};

typedef TWIconData_NT* pTWIconData;

struct TWIconsInfo 
{
   int		    Cnt;
   pTWIconData* iconData;
};

struct TWData 
{
   DWORD        unknown[7];
   TWIconsInfo* iconsInfo;
   HIMAGELIST   iconList;
};



//************************************************************************************
//
//
typedef struct 
{
   DWORD u1;
   HWND  hwnd;				// Window which handles requests about systray icons,
						    // may be, it's the handle of "TrayNotifyWnd".
   DWORD u2;
   HIMAGELIST hIList;		// Image list which contains icons itself.
} TrayDataT;

// Same as NOTIFYICONDATA
typedef struct 
{
   HWND  hWnd;
   UINT  uID;
   DWORD uCallbackMessage;
   DWORD uFlags;
} IconInfoT;

// Structure to pass requests about systray icons.
typedef struct 
{
   DWORD    sz;
   DWORD    dwFlags;
   DWORD    p2;
   int      iImage;
   DWORD    p3;
   IconInfoT* pIconInfo;
   LPWSTR   pTip;
   DWORD    szTip;
} ControlDataT;

//
//
//************************************************************************************



void DeleteTrayIcon( HWND hWnd, UINT uID )
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		     = sizeof(NOTIFYICONDATA);
	tnd.hWnd		     = hWnd;
	tnd.uID			     = uID;
	tnd.uFlags		     = 0;
	tnd.uCallbackMessage = 0;
	tnd.hIcon		     = 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
}





//---------------------------------------------------------------------------
// Returns number of icons on systray.
int getIconCount( TrayDataT* pData )
{
   return SendMessageW( pData->hwnd, 0x418, 0, 0 );
}


//---------------------------------------------------------------------------
// Returns pointer to IconInfoT struct. iIndex is icon index, from zero
// to getIconCount()-1. bFlag must be true.
IconInfoT* getIconInfo( TrayDataT* pData, int iIndex, bool bFlag )
{
   ControlDataT s;
   
   s.sz        = sizeof(s);
   s.dwFlags   = bFlag ? 0x80000010 : 0x10;
   s.pIconInfo = 0;
   SendMessageW( pData->hwnd, 0x43F, iIndex, LPARAM(&s) );
   
   return s.pIconInfo;
}

//---------------------------------------------------------------------------
// Gets tip for icon.
void getTip( TrayDataT* pData, int iIndex, LPWSTR pTip, DWORD szTip )
{
   ControlDataT s;

   s.sz      = sizeof(s);
   s.dwFlags = 0x80000002;
   s.pTip    = pTip;
   s.szTip   = szTip;
   
   SendMessageW( pData->hwnd, 0x43F, iIndex, LPARAM(&s) );
}


/*
 *
 *
 */
void EnumTrayIcons_Win2000 ( void* p_Data )
{
	PRINTF("Start EnumTrayIcons_Win2000");

	TrayDataT* pData = (TrayDataT*)p_Data;

	int cnt = getIconCount( pData );

	PRINTF("getIconCount: %d", cnt);

	for ( int i=0; i<cnt; i++ ) 
	{
		for ( int j=0; j<HideTitleList.GetCount(); j++ )
		{
			THideByRunInfo* info = (THideByRunInfo*)HideTitleList.GetInfo(j);
			if ( info==0 || info->Empty() )
				continue;
			if ( !info->m_bDeleteTrayIcon )
				continue;

			WCHAR szTip[64];
			char  buf[64];
			getTip( pData, i, szTip, sizeof(szTip) );

			PRINTF("szTip:");
			for ( int zzz=0; zzz<16; zzz++)
				PRINTF(" %02X", ((BYTE*)&szTip)+i );
			
			wcstombs( buf, szTip, sizeof(buf) );

			PRINTF("buf: %s", buf);

			if ( CompareTitle( info, buf ) )
			{
				PRINTF("CompareTitle: OK");

				IconInfoT* pInfo = getIconInfo( pData, i, true );
				DeleteTrayIcon( pInfo->hWnd, pInfo->uID );
			}
			else
			{
				PRINTF("CompareTitle: !OK");
			}
		}
	}

	PRINTF("End EnumTrayIcons_Win2000");
}


/*
 *
 *
 */
void EnumTrayIcons( )
{
	/*
	DWORD dwStartTime = GetTickCount();

	for ( int z=0; z<10000; z++ )
	{
	*/
	HWND    stw = FindWindow( TRAY_WND, 0 );
	HWND    tw  = FindWindowEx( stw, 0, TRAY_NOTIFY_WND, 0 );
	TWData* twd = (TWData*)GetWindowLong( tw, 0 );

	PRINTF("TRAY_WND: %08X    TRAY_NOTIFY_WND: %08X    twd: %08X", stw, tw,  twd);

	if ( !twd ) 
		return;

	if ( isWinNT2K )
	{
		PRINTF("isWinNT2K: yes");
		//EnumTrayIcons_Win2000( (void*)twd );
		return;
	}

	PRINTF("isWinNT4: yes");

	// We cannot just read memory of another process
	DWORD idExplorer;
	GetWindowThreadProcessId( tw, &idExplorer );

	HANDLE hExplorer = OpenProcess( PROCESS_VM_READ, FALSE, idExplorer );
	if ( !hExplorer ) 
		return;

	//Read tray window data
	TWData data;
	BOOL r = ReadProcessMemory( hExplorer, twd, &data, sizeof(data), 0 );
	if ( !r ) 
		return;

	// Read icons info
	TWIconsInfo iconsInfo;
	r = ReadProcessMemory( hExplorer, data.iconsInfo, &iconsInfo, sizeof(iconsInfo), 0 );
	if ( !r ) 
		return;

	if ( iconsInfo.Cnt==0 ) 
		return;

	// Ok, now read pointers to icons data
	pTWIconData* ppIconData = new pTWIconData[iconsInfo.Cnt];
	r = ReadProcessMemory( hExplorer, iconsInfo.iconData, ppIconData, sizeof(pTWIconData)*iconsInfo.Cnt, 0 );
	if ( !r ) 
		return;

	// Now read icons data itself
	TWIconData_NT IconData_NT;
	TWIconData_95 IconData_95;
	char buf[64];

	UINT  uID    = 0;  
	HWND  hWnd   = 0;  
	char* pszTip = 0;  

	for ( int i=0; i<iconsInfo.Cnt; i++ ) 
	{
		if ( isWinNT )
		{
			r = ReadProcessMemory( hExplorer, ppIconData[i], &IconData_NT, sizeof(TWIconData_NT), 0 );
			if ( !r ) 
				return;

			wcstombs( buf, IconData_NT.d.szTip, sizeof(buf) );
			pszTip = buf;
			hWnd   = IconData_NT.d.hWnd;
			uID    = IconData_NT.d.uID;
		}
		else
		{
			r = ReadProcessMemory( hExplorer, ppIconData[i], &IconData_95, sizeof(TWIconData_95), 0 );
			if ( !r ) 
				return;

			pszTip = IconData_95.d.szTip;
			hWnd   = IconData_95.d.hWnd;
			uID    = IconData_95.d.uID;
		}

		for ( int j=0; j<HideTitleList.GetCount(); j++ )
		{
			THideByRunInfo* info = (THideByRunInfo*)HideTitleList.GetInfo(j);
			if ( info==0 || info->Empty() )
				continue;
			if ( !info->m_bDeleteTrayIcon )
				continue;

			if ( CompareTitle( info, pszTip ) )
			{
				DeleteTrayIcon( hWnd, uID );
			}
		}
	}

	delete[] ppIconData;
	CloseHandle( hExplorer );
	/*
	}

	cout<< "Time="<<GetTickCount()-dwStartTime<<endl;
	*/
}

