/*
 * Copyright (c) 1998-2000 by KVA Software
 * All rights reserved.	
 *
 * File:      dbg.cpp
 * Contents:  CDebugPrintf class implementation
 * Author:    Vladimir Kirienko
 *
 *            http://www.uran.net/kva
 *            kirienko@geocities.com
 */
#include <time.h> 
#include <windows.h> 
#include "dbg.h"      


#ifdef USE_DEBUG_PRINTF


const char DIVIDER[10] = { 13, 10, '-', '-', '-', '-', '-', '-', 13, 10 };
const char CrLf[3]	   = { 13, 10, 0 };

const char MUTEX_NAME[]	  = "KVADebugMutex";	
const char LOG_FILENAME[] = "C:\\TMP\\Dbg.log";	// Default log-file name


CDebugPrintf debug;




/*-----------------------------------------------------------------------
 *  Function    : 
 *  Prototype   : CDebugPrintf::CDebugPrintf ()
 *  Description : 
 *  Parameters  :                   
 *  Return      : 
 *  History     : 
 */
CDebugPrintf::CDebugPrintf ()
{
	DWORD x;

	m_bUseConsole = false;
	m_hDebugMutex = OpenMutex( MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME);  
	
	// We are the first application who wants to print to log file
	if ( m_hDebugMutex==0 )
	{
		m_hDebugMutex = CreateMutex( 0, FALSE, MUTEX_NAME );

		// Save old log file (up to 2)
		char newFile1[255];
		char newFile2[255];
		
		lstrcpy( newFile1, LOG_FILENAME );
		lstrcat( newFile1, "~" );

		lstrcpy( newFile2, newFile1 );
		lstrcat( newFile2, "~" );

		CopyFile( newFile1,     newFile2, FALSE );
		CopyFile( LOG_FILENAME, newFile1, FALSE );

		// Write time stamp to log-file
		HANDLE hFile = CreateFile( LOG_FILENAME, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

		DWORD dwTime = GetTickCount();
		WriteFile( hFile, &dwTime, sizeof(dwTime),  &x, 0 );	
		WriteFile( hFile, DIVIDER, sizeof(DIVIDER), &x, 0 );	
		CloseHandle( hFile );
	}

	m_hFile = CreateFile( LOG_FILENAME, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	ReadFile( m_hFile, &m_dwStartTime, sizeof(m_dwStartTime), &x, 0 );	

	InitializeCriticalSection( &m_hLock );
}




/*-----------------------------------------------------------------------
 *  Function    : 
 *  Prototype   : CDebugPrintf::CDebugPrintf ()
 *  Description : 
 *  Parameters  :                   
 *  Return      : 
 *  History     : 
 */
CDebugPrintf::~CDebugPrintf ()
{
	CloseLog();
}





/*-----------------------------------------------------------------------
 *  Function    : 
 *  Prototype   : void CDebugPrintf::CloseLog ()
 *  Description : Closes log file and releases handles. 
 *  Parameters  :                   
 *  Return      : void
 *  History     : 
 */
void CDebugPrintf::CloseLog ()
{
	CloseHandle( m_hFile );

	DeleteCriticalSection( &m_hLock );
	ReleaseMutex( m_hDebugMutex );
	CloseHandle( m_hDebugMutex );
}





/*-----------------------------------------------------------------------
 *  Function    : 
 *  Prototype   : void CDebugPrintf::printf ( const char *fmt, ... )
 *  Description : Prints formatted string to log-file
 *  Parameters  : const char *fmt - format string
                  ...             - list of parameters
 *  Return      : void
 *  History     : 
 */
void CDebugPrintf::printf ( const char *fmt, ... )
{
	EnterCriticalSection( &m_hLock );

	// Format string
	char buffer[1024*50];
	va_list arglist;
	va_start( arglist, fmt );
	wvsprintf( buffer, fmt, arglist );
	va_end(arglist);

	// Format time stamp
	DWORD x;
	char  buf[256];

	char szTimeBuf[32];
	char szDateBuf[32];

    _tzset();
	_strtime( szTimeBuf );
	_strdate( szDateBuf );

	DWORD dwDiffTime = GetTickCount()-m_dwStartTime;
	
	wsprintf( buf, "%d.%03d %s %s  ", dwDiffTime/1000, dwDiffTime%1000, szDateBuf, szTimeBuf );

	// Write time stamp and formatted string to log-file
	SetFilePointer( m_hFile, 0, 0, FILE_END );
	WriteFile( m_hFile,	buf,    lstrlen(buf),    &x, 0 );	
	WriteFile( m_hFile,	buffer, lstrlen(buffer), &x, 0 );	
	WriteFile( m_hFile,	CrLf,   lstrlen(CrLf),   &x, 0 );	

	if ( m_bUseConsole )
	{
		WriteFile( GetStdHandle(STD_OUTPUT_HANDLE),	buf,    lstrlen(buf),    &x, 0 );	
		WriteFile( GetStdHandle(STD_OUTPUT_HANDLE),	buffer, lstrlen(buffer), &x, 0 );	
		WriteFile( GetStdHandle(STD_OUTPUT_HANDLE),	CrLf,   lstrlen(CrLf),   &x, 0 );	
	}

	LeaveCriticalSection( &m_hLock );
}





/*-----------------------------------------------------------------------
 *  Function    : ShowConsole
 *  Prototype   : void CDebugPrintf::ShowConsole() 
 *  Description : Shows log-console
 *  Parameters  :                   
 *  Return      : void 
 *  History     : 
 */
void CDebugPrintf::ShowConsole() 
{
	EnterCriticalSection( &m_hLock );
	
	m_bUseConsole = true;
	AllocConsole();
	
	LeaveCriticalSection( &m_hLock );
}


/*-----------------------------------------------------------------------
 *  Function    : HideConsole
 *  Prototype   : void CDebugPrintf::HideConsole() 
 *  Description : Hides log-console
 *  Parameters  :                   
 *  Return      : void 
 *  History     : 
 */
void CDebugPrintf::HideConsole() 
{
	EnterCriticalSection( &m_hLock );
	
	m_bUseConsole = true;
	AllocConsole();
	
	LeaveCriticalSection( &m_hLock );
}



/*-----------------------------------------------------------------------
 *  Function    : DisplayLastError
 *  Prototype   : void CDebugPrintf::DisplayLastError() 
 *  Description : Print description for error returned by GetLastError function
 *  Parameters  :                   
 *  Return      : void 
 *  History     : 
 */
void CDebugPrintf::DisplayLastError() 
{
	EnterCriticalSection( &m_hLock );

	char* lpMsgBuf;

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,    
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,    
		0,    
		NULL );

	int len = lstrlen(lpMsgBuf)-1;

	if ( len>0 )
	{
		while ( len>=0 && (lpMsgBuf[len]==0x0d || lpMsgBuf[len]==0x0a) )
			len--;
		lpMsgBuf[len+1] = 0; 
		printf ( "GetLastError(): %s", (char*)lpMsgBuf );
		LocalFree( lpMsgBuf ); // Free the buffer.
	}

	LeaveCriticalSection( &m_hLock );
}



#endif
