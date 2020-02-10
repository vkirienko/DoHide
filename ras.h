#ifndef __RAS_H__
#define __RAS_H__

#include <ras.h>
#include <raserror.h>


typedef DWORD (*fnRasEnumEntries)(LPCSTR,LPCSTR,LPRASENTRYNAME,LPDWORD,LPDWORD);


class CRas
{
public:
	CRas () : hRAS(0) {}

	void EnumEntries ( HMENU hMenu, int iMenuItem );
	bool Dial ( char* szRasEntryName );

private:
	HINSTANCE		 hRAS;
	fnRasEnumEntries _RasEnumEntries; 

	void LoadRasLibrary ();
	void FreeRasLibrary ();
};

extern CRas ras;

#endif