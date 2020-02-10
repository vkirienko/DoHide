#include "stdafx.h"

#include "DoHide.h"
#include "registry.h"
#include "hotkeys.h"

const char HK_SECTION[] = "HotKeys\\";
const char HK_KEY[]		= "HotKey%02d";
const char HK_ADDMENU[] = "AddMenu%02d";

const BYTE TranslateTable[MAX_USED_HOTKEY+1] = 
{
	 1,  2,  3,  4,  5,  6,  7,  8,  9, 35,
	10, 20, 21, 22, 23, 24, 25, 26, 27, 12,
	13, 33, 14, 34, 15, 11, 16, 17, 18, 19,
	30, 32, 28, 29, 31
};


THotKeys HotKeys;

THotKeys::THotKeys()
{
	ZeroMemory(&m_uHotKey[0],sizeof(m_uHotKey));
	ZeroMemory(&m_uAddToMenu[0],sizeof(m_uAddToMenu));
}


THotKeys::~THotKeys()
{
}


void THotKeys::LoadFromRegistry()
{
	char s[30];
	char section[60];
	lstrcpy(section,SECTION_DOHIDE);
	lstrcat(section,HK_SECTION);

	ZeroMemory(&m_uHotKey[0],sizeof(m_uHotKey));
	for ( int i=0; i<COUNT_HOTKEY; i++ ) 
	{
		wsprintf(s,HK_KEY,i);
		ReadKey( HKEY_CURRENT_USER, section, s, m_uHotKey[i], 0 );
		wsprintf(s,HK_ADDMENU,i);
		ReadKey( HKEY_CURRENT_USER, section, s, m_uAddToMenu[i], i<3?1:0 );
	}
}


void THotKeys::StoreToRegistry()
{
	char s[30];
	char section[60];
	lstrcpy(section,SECTION_DOHIDE);
	lstrcat(section,HK_SECTION);

	for ( int i=0; i<COUNT_HOTKEY; i++ ) 
	{	
		wsprintf(s,HK_KEY,i);
		WriteKey(HKEY_CURRENT_USER, section, s, m_uHotKey[i] );
		wsprintf(s,HK_ADDMENU,i);
		WriteKey(HKEY_CURRENT_USER, section, s, m_uAddToMenu[i] );
	}
}


void THotKeys::RegisterAllHotKey ()
{
	for ( int i=0; i<COUNT_HOTKEY; i++ ) 
	{
		int j = Translate(i);

		if ( m_uHotKey[j] )
			 RegisterHotKey( hMainDlg, j, GetMod(m_uHotKey[j]), m_uHotKey[j]&0x000000ff );
	}
}


void THotKeys::UnregisterAllHotKey ()
{
	for ( int i=0; i<COUNT_HOTKEY; i++ ) 
	{
		UnregisterHotKey( hMainDlg, Translate(i) );
	}
}


UINT THotKeys::GetMod ( DWORD hk )
{
	UINT mod = 0;
	if ( (hk>>8)&HOTKEYF_CONTROL ) mod |= MOD_CONTROL;
	if ( (hk>>8)&HOTKEYF_ALT     ) mod |= MOD_ALT;
	if ( (hk>>8)&HOTKEYF_SHIFT   ) mod |= MOD_SHIFT;
	return mod;
}


int THotKeys::Translate( int i )
{
	return TranslateTable[i]-1;
}
