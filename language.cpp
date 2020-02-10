#include "StdAfx.h"

#include "DoHide.h"
#include "globals.h"
#include "resource.h"

const char SZ_LANG_END[] = "*.lng";



/*
 *
 *
 */
bool LoadLanguageDll( UINT id )
{
	WIN32_FIND_DATA wfd;

	char s[128];
	char fName[MAX_PATH];

 	GetModuleFileName( hInst, fName, sizeof(fName) );
	fName[lstrlen(fName)-4] = 0;
	lstrcat(fName,SZ_LANG_END);
	
	HANDLE hFindFile = FindFirstFile( fName, &wfd );

	if ( hFindFile==INVALID_HANDLE_VALUE )
		return false;

	do
	{
		hResourceInst = LoadLibrary( wfd.cFileName );

		if ( hResourceInst!=0 )
		{
			LoadStr( IDS_LANGUAGE_ID, s, sizeof(s) );
			if ( id==(UINT)(s[0]-'0') )
			{
				FindClose( hFindFile );
				return true;
			}
			FreeLibrary(hResourceInst);
		}
	}
	while ( FindNextFile(hFindFile,&wfd) );

	FindClose( hFindFile );
	return false;
}


/*
 *
 *
 */
void FillLanguageComboBox ( HWND hDlg, UINT idLangCombo )
{
	WIN32_FIND_DATA wfd;

	char s[64];
	char fName[MAX_PATH];
	
	GetModuleFileName( hInst, fName, sizeof(fName) );
	fName[lstrlen(fName)-4] = 0;
	lstrcat(fName,SZ_LANG_END);

	HANDLE hFindFile = FindFirstFile( fName, &wfd );

	if ( hFindFile==INVALID_HANDLE_VALUE )
		return;

	HWND hComboWnd = GET(IDC_LANG_COMBO);

	do
	{
		HINSTANCE hLib = LoadLibrary( wfd.cFileName );
		LoadString( hLib, IDS_LANGUAGE, s, sizeof(s) );

		//int cnt = SendMessage( hComboWnd, CB_GETCOUNT, 0, 0 ); 
		int cnt = SendMessage( hComboWnd, CB_ADDSTRING, 0, (long)s );

		LoadString( hLib, IDS_LANGUAGE_ID, s, sizeof(s) );
		SendMessage( hComboWnd, CB_SETITEMDATA, cnt, (s[0]-'0') ); 

		FreeLibrary( hLib );
	}
	while ( FindNextFile(hFindFile,&wfd) );

	FindClose( hFindFile );

	//
	//
	//
	int cnt = SendMessage( hComboWnd, CB_GETCOUNT, 0, 0 ); 
	for ( int i=0; i<cnt; i++ )
	{
		UINT id = SendMessage( hComboWnd, CB_GETITEMDATA, i, 0 ); 
		if ( id==uKey[0] )
		{
			SendMessage( hComboWnd, CB_SETCURSEL, i, 0 ); 
			break;
		}
	}
}