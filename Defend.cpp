#include "stdafx.h"

#include "DoHide.h"
#include "register.h"
#include "rsa.h"


CLibrary* lib;


CLibrary::CLibrary()
{
	ZeroMemory( this, sizeof(CLibrary) );
	
	char s[1024];
	char* ptr;

	ptr = GetFunctionName( s, 1 );
    if ( !(hKernel = LoadLibrary( ptr )) )
	    return;

	ptr = GetFunctionName( s, 2 );
	fp_GetSystemTime = (__GetSystemTime)GetProcAddress( hKernel, ptr );
	ZeroMemory( s, sizeof(s) );
}


CLibrary::~CLibrary()
{
	FreeLibrary( hKernel );
}


