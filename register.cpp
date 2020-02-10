#include "stdafx.h"

#include "DoHide.h"
#include "registry.h"
#include "register.h"
#include "GenKey.h"
#include "globals.h"


//#define USE_INLINE		__forceinline
#define USE_INLINE		



#define MAGIC_NUMBER 0x89163901 

const char DATA1[]	 = "Data1";
const char DATA2[]	 = "Data2";
// Dummy
const char DATA3[]	 = "Data3";
// Dummy

const char INT_FMT[] = "%d";

TRegistration* Register;
CSecurity*	   Security;



TRegistration::TRegistration( ) 
{
	ZeroMemory( this, sizeof(TRegistration) );

	m_pGetRegistrationNameCRC = &GetRegistrationNameCRC;

	crcgen( (unsigned long*)&crcTable );
	LoadFromRegistry();

	UpdateDayCounter();
}

TRegistration::~TRegistration( ) 
{
	StoreToRegistry( );
}

void TRegistration::LoadFromRegistry( )
{
	DWORD dwSize = sizeof(m_szRegCode);
	ReadKey(HKEY_LOCAL_MACHINE, SECTION_DOHIDE, DATA1, (BYTE**)&m_szRegCode,  &dwSize );
	dwSize = sizeof(m_szUserName);
	ReadKey(HKEY_LOCAL_MACHINE, SECTION_DOHIDE, DATA2, (BYTE**)&m_szUserName, &dwSize );
	if ( m_szRegCode[0]!=0 )
		wsprintf(m_szRegCode,INT_FMT,atoi(m_szRegCode)^MAGIC_NUMBER);

	// Dummy
	dwSize = sizeof(m_InstallTime);
	ReadKey(HKEY_LOCAL_MACHINE, SECTION_DOHIDE, DATA3, (BYTE**)&m_InstallTime, &dwSize );
	// Dummy
}

void TRegistration::StoreToRegistry( )
{
	if ( m_szRegCode[0]!=0 )
		wsprintf(m_szRegCode,INT_FMT,atoi(m_szRegCode)^MAGIC_NUMBER);
	WriteKey(HKEY_LOCAL_MACHINE, SECTION_DOHIDE, DATA1, (BYTE*)m_szRegCode,    lstrlen(m_szRegCode)+1);
	WriteKey(HKEY_LOCAL_MACHINE, SECTION_DOHIDE, DATA2, (BYTE*)m_szUserName,   lstrlen(m_szUserName)+1);
	
	// Dummy
	WriteKey(HKEY_LOCAL_MACHINE, SECTION_DOHIDE, DATA3, (BYTE*)&m_InstallTime, sizeof(FILETIME));
	// Dummy
}


USE_INLINE bool TRegistration::IsValidRegistration()
{
	unsigned long crc2 = abs(atoi(m_szRegCode));
	unsigned long crc1 = abs(getcrc(m_szUserName));
	return crc1==crc2;
}

unsigned long TRegistration::getcrc( char* szRegName )
{
	return abs(::getcrc( crcTable, szRegName, lstrlen(szRegName) ));
}

USE_INLINE unsigned long TRegistration::get_RegCode( )
{
	return abs(atoi(m_szRegCode));
}

USE_INLINE unsigned long TRegistration::get_RegNameCRC( )
{
	return abs(::getcrc( crcTable, m_szUserName, lstrlen(m_szUserName) ));
}





#ifdef _DEBUG
bool IS_REGISTRED()
{
	return 
		( 
			Register->IsValidRegistration() && 
			(Register->get_RegCode()==(*Register->m_pGetRegistrationNameCRC)() || Register->get_RegCode()==612686236) 
		);
}
#endif



void TRegistration::UpdateDayCounter()
{
	m_DayCounter = ((uKey[4]>>24) & 0x000000FF);
	m_DayOfRun   = ((uKey[4]>>16) & 0x000000FF);

	SYSTEMTIME SystemTime;

	(*lib->fp_GetSystemTime)( &SystemTime );

	if ( SystemTime.wDayOfWeek!=m_DayOfRun )
		m_DayCounter++;

	m_DayOfRun = (BYTE)SystemTime.wDayOfWeek;

	uKey[4] &= 0x0000FFFF;
	uKey[4] |= (((long)m_DayCounter)<<24 & 0xFF000000);
	uKey[4] |= (((long)m_DayOfRun)<<16 & 0x00FF0000);
}










/*
 *
 *
 */
BYTE* CSecurity::computeHash(char* strIn, BYTE hash[16])
{
	MD5_CTX context;

	MD5Init(&context);
	MD5Update(&context, (BYTE*)strIn, lstrlen(strIn)*sizeof(char));
	MD5Final(hash, &context);
	return hash;
}

/*
 *
 *
 */
bool CSecurity::ReadPassword()
{
	DWORD dwHashSize = sizeof(m_hash);
	
	LONG lRet = ReadKey( HKEY_CURRENT_USER, SECTION_DOHIDE, SZ_PASSWORD, (BYTE**)&m_hash, &dwHashSize );

	if ( lRet!=ERROR_SUCCESS || dwHashSize!=sizeof(m_hash) ) 
	{
		//pasword hash has invalid size
		return false;
	}
	return true;
}
