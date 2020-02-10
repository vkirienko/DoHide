#ifndef __REGISTRY_H__
#define __REGISTRY_H__

extern void ReadKey( HKEY hKey, const char* Section, const char* ValueName, UINT& Value, UINT DefValue=0 );
extern void ReadKey( HKEY hKey, const char* Section, const char* ValueName, char** Value, DWORD size );
extern LONG ReadKey( HKEY hKey, const char* Section, const char* ValueName, BYTE** BinaryValue, DWORD* size );

extern void WriteKey( HKEY hKey, const char* Section, const char* ValueName, UINT Value );
extern void WriteKey( HKEY hKey, const char* Section, const char* ValueName, char* Value );
extern void WriteKey( HKEY hKey, const char* Section, const char* ValueName, BYTE* Value, int BinarySize );

extern void DeleteKey( HKEY hKey, const char* Section, const char* ValueName );


#endif