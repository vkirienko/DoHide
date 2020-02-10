#include "StdAfx.h"

void ReadKey( HKEY hKey, const char* Section, const char* ValueName, UINT& Value, UINT DefValue )
{
	HKEY  hkResult;
	DWORD size = sizeof(DWORD);
	RegCreateKey( hKey, Section, &hkResult ); 
	if ( RegQueryValueEx( hkResult, ValueName, 0, 0, (BYTE*)&Value, &size ) )
		Value = DefValue;
	RegCloseKey( hkResult );
}

void ReadKey( HKEY hKey, const char* Section, const char* ValueName, char** Value, DWORD size )
{
	HKEY  hkResult; 
	RegCreateKey( hKey, Section, &hkResult ); 
	RegQueryValueEx( hkResult, ValueName, 0, 0, (BYTE*)Value, &size ); 				
	RegCloseKey( hkResult );
}

LONG ReadKey( HKEY hKey, const char* Section, const char* ValueName, BYTE** BinaryValue, DWORD* size )
{
	HKEY  hkResult; 
	RegCreateKey( hKey, Section, &hkResult ); 
	LONG lRet = RegQueryValueEx( hkResult, ValueName, 0, 0, (BYTE*)BinaryValue, size ); 				
	RegCloseKey( hkResult );
	return lRet;
}



/*
 *
 *
 */
void WriteKey( HKEY hKey, const char* Section, const char* ValueName, UINT Value )
{
	HKEY  hkResult;
	RegCreateKey( hKey, Section, &hkResult ); 
	RegSetValueEx( hkResult, ValueName, 0, REG_DWORD_LITTLE_ENDIAN, (BYTE*)&Value, sizeof(UINT) ); 				
	RegCloseKey( hkResult );
}

void WriteKey( HKEY hKey, const char* Section, const char* ValueName, char* Value )
{
	HKEY  hkResult;
	RegCreateKey( hKey, Section, &hkResult ); 
	RegSetValueEx( hkResult, ValueName, 0, REG_SZ, (BYTE*)Value, lstrlen(Value)+1 ); 				
	RegCloseKey( hkResult );
}

void WriteKey( HKEY hKey, const char* Section, const char* ValueName, BYTE* Value, int BinarySize )
{
	HKEY  hkResult;
	RegCreateKey( hKey, Section, &hkResult ); 
	RegSetValueEx( hkResult, ValueName, 0, REG_BINARY, (BYTE*)Value, BinarySize ); 				
	RegCloseKey( hkResult );
}


/*
 *
 *
 */
void DeleteKey( HKEY hKey, const char* Section, const char* ValueName )
{
	HKEY hkResult;
	RegOpenKeyEx(hKey,Section,0,KEY_ALL_ACCESS,&hkResult);
	RegDeleteValue( hkResult, ValueName ); 				
	RegCloseKey( hkResult );
}
