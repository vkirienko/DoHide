#include "StdAfx.h"

#include "register.h"

#include "..\\rsa\\rsaeuro.h"
#include "..\\rsa\\rsa.h"
#include "..\\rsa\\public_key.cpp"


//	612686236 - ex-USSR registration

/*
 *
 *
 */
char* GetFunctionName( void* decr_str, int fnId )
{
	unsigned int decryptedLength;

	// Decrypt with public key 
	RSAPublicDecrypt( (unsigned char*)decr_str, &decryptedLength, (unsigned char*)FunctionNames, sizeof(bin_FunctionNames), publicKey);

	return ((char*)decr_str)+25*fnId;
}
 

/*
 *
 *
 */
unsigned long GetRegistrationNameCRC( )
{
	char  s[4096];
	char* ptr = GetFunctionName( s, 0 );	// get license file name

	char decryptedString[1024];
	unsigned int status, decryptedLength;

	decryptedString[0] = 0;
	
	HANDLE hFile = CreateFile( ptr, GENERIC_READ, 0, 0,	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	ZeroMemory( s, sizeof(s) );

	if ( hFile==INVALID_HANDLE_VALUE )
		return 0;

	DWORD dwNumberOfBytesRead;
	ReadFile( hFile, &s, 4096, &dwNumberOfBytesRead, 0 );

	CloseHandle( hFile );

	// Decrypt with public key 
	status = RSAPublicDecrypt( (unsigned char*)decryptedString, &decryptedLength, (unsigned char*)&s[1893], 128, publicKey);
	if (status) 
	{
		return 0;
	}

	// Display decrypted string 
	decryptedString[decryptedLength] = 0;

	unsigned long crc = Register->getcrc( decryptedString );
	ZeroMemory( decryptedString, sizeof(decryptedString) );

	return crc;
}
