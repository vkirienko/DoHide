#include "stdafx.h"

#include <ras.h>
#include <raserror.h>
#include <Wininet.h>

#include "DoHide.h"
#include "globals.h"
#include "menu.h"
#include "ras.h"


const char szRasDLL[]         = "rasapi32.dll";
const char szRasEnumEntries[] = "RasEnumEntriesA";


const char szWinNTCmdLine[] = "-d \"%s\"";
const char szWinNTDialer[]  = "RasPhone.exe"; 
const char szWin95CmdLine[] = "rnaui.dll,RnaDial %s";
const char szWin95Dialer[]  = "RunDll.exe"; 



CRas ras;

/*
 *
 *
 */
void CRas::LoadRasLibrary ()
{
	hRAS = LoadLibrary( szRasDLL );
	_RasEnumEntries = ( hRAS ) ? _RasEnumEntries = (fnRasEnumEntries)GetProcAddress( hRAS, szRasEnumEntries ) : 0;
}

/*
 *
 *
 */
void CRas::FreeRasLibrary ()
{
	if ( hRAS )
		FreeLibrary( hRAS );
}

/*
 *
 *
 */
void CRas::EnumEntries(HMENU hMenu, int iMenuItem)
{
	RASENTRYNAME* lpRasEntryName = (RASENTRYNAME*)GlobalAlloc(GPTR, sizeof(RASENTRYNAME));
	lpRasEntryName->dwSize = sizeof(RASENTRYNAME);

	DWORD nRet;
	DWORD cb;
	DWORD cEntries;

	LoadRasLibrary();

	if ( _RasEnumEntries )
	{
	    cb = sizeof(RASENTRYNAME);
		if ( (nRet = (*_RasEnumEntries)(NULL, NULL, lpRasEntryName, &cb, &cEntries))==ERROR_BUFFER_TOO_SMALL )
		{
			GlobalFree( lpRasEntryName );
			lpRasEntryName = (RASENTRYNAME*)GlobalAlloc(GPTR, cb);
			lpRasEntryName->dwSize = sizeof(RASENTRYNAME);
		}
 
		// Calling RasEnumEntries to enumerate the phone-book entries    
		nRet = (*_RasEnumEntries)(NULL, NULL, lpRasEntryName, &cb, &cEntries);
 
		if (nRet == ERROR_SUCCESS)
		{
			RASENTRYNAME* lpTempRasEntryName = lpRasEntryName;
			for ( DWORD i=0; i<cEntries; i++,lpTempRasEntryName++ )
			{
				int len = lstrlen(lpTempRasEntryName->szEntryName);
				char* ptr = new char[len+1];
				lstrcpy( ptr, lpTempRasEntryName->szEntryName );

				MENUITEMINFO mInfo;
				mInfo.cbSize     = sizeof(MENUITEMINFO);
				mInfo.fMask      = MIIM_ID|MIIM_TYPE|MIIM_DATA;
				mInfo.fType      = MFT_OWNERDRAW;
				mInfo.hSubMenu   = 0;
				mInfo.dwItemData = CustomMenu.AddItem( (int)ptr, MIT_DIALUP_NAME );
				mInfo.wID        = WM_EXECUTE_CUSTOM_MENU + mInfo.dwItemData; 
				InsertMenuItem(hMenu, 2+iMenuItem, TRUE, &mInfo);
				iMenuItem++;
			}
		}
	}

	FreeRasLibrary();

	GlobalFree( lpRasEntryName );
}


/*
 *
 *
 */
bool CRas::Dial( char* szRasEntryName )
{
	char s[64];
	SHELLEXECUTEINFO sei;
	
	ZeroMemory(&sei,sizeof(SHELLEXECUTEINFO));

	if ( isWinNT )
	{
		wsprintf( s, szWinNTCmdLine, szRasEntryName );
		sei.lpFile = szWinNTDialer; 
	}
	else
	{
		wsprintf( s, szWin95CmdLine, szRasEntryName );
		sei.lpFile = szWin95Dialer; 
	}

	sei.cbSize       = sizeof(SHELLEXECUTEINFO);
	sei.nShow        = SW_SHOWNORMAL;
	sei.lpParameters = s;

	return ShellExecuteEx(&sei)==TRUE;
}
