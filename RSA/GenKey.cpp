#include <windows.h>
#include <stdio.h>

#include "rsaeuro.h"
#include "rsa.h"

#define PrintMessage(message) \
{                 \
	puts(message);  \
	fflush(stdout); \
}

static void PrintMessage PROTO_LIST((char *));
static void PrintError PROTO_LIST((char *, int));
static int GetCommand PROTO_LIST((char *, unsigned int, char *));
static char *upr PROTO_LIST ((char *s));


//  25 char
char input1[] = 
{ 
	'D','o','H','i','d','e','R','.','d','l','l',0  ,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'k','e','r','n','e','l','3','2','.','d','l','l',0  ,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'G','e','t','S','y','s','t','e','m','T','i','m','e',0  ,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

R_RSA_PUBLIC_KEY	publicKey;
R_RSA_PRIVATE_KEY	privateKey;






unsigned long crcTable[256];

/****************************************************************************/
void crcgen( unsigned long* crcTable )
{
	unsigned long	crc, poly;
	int	i, j;

	poly = 0xEDB88320L;
	for (i=0; i<256; i++) {
		crc = i;
		for (j=8; j>0; j--) {
			if (crc&1) {
				crc = (crc >> 1) ^ poly;
			} else {
				crc >>= 1;
			}
		}
		crcTable[i] = crc;
	}
}


/****************************************************************************/
unsigned long getcrc( unsigned long* crcTable, char* s, int size )
{
	unsigned long crc = 0xFFFFFFFF;

	for ( int cnt=0; cnt!=size; cnt++ ) 
	{
		int c = s[cnt];
		crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^c) & 0xFF ];
	}

	return ( crc^0xFFFFFFFF );
}








void WriteFile( FILE* file, unsigned char* ptr, int size, char* varType, char* varName )
{
	fprintf( file, "unsigned char bin_%s[%d] =\n{\n", varName, size );
	for ( int i=0; i<=size/12; i++ )
	{ 
		fprintf( file, "\t" );
		for ( int j=0; j<12 && (i*12+j)<size; j++ )
		{
			fprintf( file, "0x%02x", ptr[i*12+j] );
			if ( (i*12+j+1)<size )
				fprintf( file, ", " );
		}

		fprintf( file, "\n" );
	}
	fprintf( file, "};\n\n" );
	fprintf( file, "%s* %s = (%s*)&bin_%s;\n\n", varType, varName, varType, varName );
}


void DoGenerateKeys()
{
	R_RANDOM_STRUCT randomStruct;
	R_RSA_PROTO_KEY protoKey;
	int status;

	// Initialise random structure ready for keygen 
	R_RandomCreate(&randomStruct);
	
	// Initialise prototype key structure 
	protoKey.bits = 1024;
	protoKey.useFermat4 = 1;
	
	// Generate keys 
	status = R_GeneratePEMKeys(&publicKey, &privateKey, &protoKey, &randomStruct);
	if (status)
	{
		printf("R_GeneratePEMKeys failed with %d\n", status);
		return;
	}
 
	FILE* file = fopen("public_key.cpp","wb");

	char output[MAX_RSA_MODULUS_LEN+2];
	unsigned int outsize;
	
	// Encrypt string with private key 
	if ( RSAPrivateEncrypt( (unsigned char*)output, &outsize, (unsigned char*)input1, sizeof(input1), &privateKey) )
	{
		printf("RSAPrivateEncrypt failed with %x\n", status);
		return;
	}

	WriteFile( file, (unsigned char*)&output, outsize, "char", "FunctionNames" );
	WriteFile( file, (unsigned char*)&publicKey, sizeof(publicKey)/sizeof(unsigned char), "R_RSA_PUBLIC_KEY", "publicKey" );
	
    fclose( file );


	file = fopen("public.key","wb");
	if ( (fwrite(&publicKey, sizeof(publicKey), 1, file)) != 1) 
	{
		PrintMessage("ERROR: Cannot Write Public Key to File.");
	} 
	fclose( file );

	file = fopen("private.key","wb");
	if ( (fwrite(&privateKey, sizeof(privateKey), 1, file)) != 1 )
		PrintMessage("ERROR: Cannot Write Private Key to File.");
	fclose( file );
}


void DoLoadKeys()
{
	FILE* file = fopen("public.key","rb");
	if ( (fread(&publicKey, sizeof(publicKey), 1, file)) != 1) 
	{
		PrintMessage("ERROR: Cannot Read Public Key to File.");
	} 
	fclose( file );

	file = fopen("private.key","rb");
	if ( (fread(&privateKey, sizeof(privateKey), 1, file)) != 1 )
		PrintMessage("ERROR: Cannot Read Private Key to File.");
	fclose( file );
}



void DoSealFile()
{
	char filename[64];

	while ( 1 ) 
	{
		if ( !GetCommand(filename, sizeof (filename), "  Enter registration name") )
			return;

		if ( filename[0] == '-' && filename[1] == '\0' ) 
		{
			/* use stdout */
			return;
		}

		break;
	}

	char* demostring = filename;
	char encryptedString[MAX_RSA_MODULUS_LEN+2];
	char decryptedString[256];
	unsigned int status, encryptedLength, decryptedLength;
	
	DoLoadKeys();

	memset( encryptedString, 0, sizeof(encryptedString) );
	memset( decryptedString, 0, sizeof(decryptedString) );
	
	// Encrypt string with private key 
	status = RSAPrivateEncrypt( (unsigned char*)encryptedString, &encryptedLength, (unsigned char*)demostring,
		lstrlen(demostring), &privateKey);
	if (status)
	{
		printf("RSAPrivateEncrypt failed with %x\n", status);
		return;
	}
	
	// Decrypt with public key 
	status = RSAPublicDecrypt( (unsigned char*)decryptedString, &decryptedLength, (unsigned char*)encryptedString,
		encryptedLength, &publicKey);
	if (status)
	{
		printf("RSAPublicDecrypt failed with %x\n", status);
		return;
	}
	
	// Display decrypted string 
	decryptedString[decryptedLength+1] = (char) "\0";
	printf("User name = %s\n", decryptedString);
	printf("Registration code = %d\n", abs( getcrc( crcTable, decryptedString, lstrlen(decryptedString) )));


	FILE* file = fopen("DoHideR.dll","r+b");
	fpos_t pos = 1893;
	if ( fsetpos(file, &pos )!=0 )
	{
		PrintMessage("ERROR: Cannot Write Registration Key to File. (seek)");
	}
	if ( (fwrite(encryptedString, encryptedLength, 1, file)) != 1) 
	{
		PrintMessage("ERROR: Cannot Write Registration Key to File. (write)");
	} 
	fclose( file );

}


void DoReadRegName()
{
	char filename[64];

	char* demostring = filename;
	char encryptedString[MAX_RSA_MODULUS_LEN+2];
	char decryptedString[256];
	unsigned int status, encryptedLength, decryptedLength;
	
	DoLoadKeys();

	memset( encryptedString, 0, sizeof(encryptedString) );
	memset( decryptedString, 0, sizeof(decryptedString) );


	HANDLE hFile = CreateFile( "DoHideR.dll", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( hFile==INVALID_HANDLE_VALUE )
		return;

	DWORD dwFileSizeHigh;
	encryptedLength = GetFileSize( hFile, &dwFileSizeHigh );

	DWORD dwNumberOfBytesRead;
	ReadFile( hFile, &encryptedString, encryptedLength, &dwNumberOfBytesRead, 0 );

	CloseHandle( hFile );

	// Decrypt with public key 
	status = RSAPublicDecrypt( (unsigned char*)decryptedString, &decryptedLength, (unsigned char*)encryptedString, encryptedLength, &publicKey);
	if (status)
	{
		printf("RSAPublicDecrypt failed with %x\n", status);
		return; 
	}
	
	// Display decrypted string 
	decryptedString[decryptedLength+1] = (char) "\0";
	printf("Decrypted string: %s\n", decryptedString);
}


/*
void RSAExample()
{
	char demostring[] = "Don Donaldson reg";
	char encryptedString[MAX_RSA_MODULUS_LEN+2];
	char decryptedString[256];
	unsigned int status, encryptedLength, decryptedLength;
	
	memset( encryptedString, 0, sizeof(encryptedString) );
	memset( decryptedString, 0, sizeof(decryptedString) );
	
	// Encrypt string with private key 
	status = RSAPrivateEncrypt( (unsigned char*)encryptedString, &encryptedLength, (unsigned char*)demostring,
		lstrlen(demostring), &privateKey);
	if (status)
	{
		printf("RSAPrivateEncrypt failed with %x\n", status);
		return;
	}
	
	// Decrypt with public key 
	status = RSAPublicDecrypt( (unsigned char*)decryptedString, &decryptedLength, (unsigned char*)encryptedString,
		encryptedLength, &publicKey);
	if (status)
	{
		printf("RSAPublicDecrypt failed with %x\n", status);
		return;
	}
	
	// Display decrypted string 
	decryptedString[decryptedLength+1] = (char) "\0";
	printf("Decrypted string: %s\n", decryptedString);
}





void RSAExample2()
{
	R_RANDOM_STRUCT randomStruct;

	char demostring[] = "Test string for RSA functions #2";
	char encryptedString[MAX_RSA_MODULUS_LEN+2];
	char decryptedString[256];
	unsigned int status, encryptedLength, decryptedLength;

	// Initialise Random structure 
	R_RandomCreate(&randomStruct);

	memset( encryptedString, 0, sizeof(encryptedString) );
	memset( decryptedString, 0, sizeof(decryptedString) );
	
	// Encrypt string with public key 
	status = RSAPublicEncrypt( (unsigned char*)encryptedString, &encryptedLength, (unsigned char*)demostring,
		lstrlen(demostring), &publicKey, &randomStruct);
	if (status)
	{
		printf("RSAPublicEncrypt failed with %x\n", status);
		return;
	}
	
	// Decrypt with public key 
	status = RSAPrivateDecrypt( (unsigned char*)decryptedString, &decryptedLength,
		(unsigned char*)encryptedString, encryptedLength, &privateKey);
	if (status)
	{
		printf("RSAPrivateDecrypt failed with %x\n", status);
		return;
	}

	// Display decryp ted string 
	decryptedString[decryptedLength+1] = (char) "\0";
	printf("Decrypted string: %s\n", decryptedString);
}
*/

int main( int argc, char* argv[] )
{
	char command[80];
	int done = 0;

	crcgen( crcTable );

	while ( !done ) 
	{
		PrintMessage("");
		PrintMessage("[G] - Generate a keypair");
		PrintMessage("[L] - Load keypair");
		PrintMessage("[C] - Create registration file");
		PrintMessage("[R] - Read registration name");
		PrintMessage("[Q] - Quit");
		GetCommand(command, sizeof (command), "  Enter choice");

		upr(command);
		switch(*command) 
		{
		case 'G':
			DoGenerateKeys();
			break;

		case 'L':
			DoLoadKeys();
			break;

		case 'C':
			DoSealFile();
			break;

		case 'R':
			DoReadRegName();
			break;

		case '\0':
		case 'Q':
			done = 1;
			break;

		default:
			PrintError("ERROR: Unrecognized command.  Try again.", 0);
			break;
		}
	}

	return 0;
}


static int GetCommand(char *command, unsigned int maxCommandSize, char *prompt)
{
	unsigned int i;

	printf("%s (blank to cancel): \n", prompt);
	fflush(stdout);

	fgets(command, maxCommandSize, stdin);

	/* Replace the line terminator with a '\0'.	 */

	for (i = 0; command[i] != '\0'; i++) {
		if (command[i] == '\012' || command[i] == '\015' ||	i == (maxCommandSize - 1)) {
			command[i] = '\0';
			break;
		}
	}
	return(strlen(command));
}

static char *upr(char *s)
{
	char *p = s;

	while(*s) {
		if((*s >= 'a') && (*s <= 'z'))
			*s += 'A'-'a';
		s++;
	}

	return p;
}

/* If type is zero, simply print the task string, otherwise convert the
		 type to a string and print task and type.
 */
static void PrintError(char *task, int type)
{
	char *typeString, *msg[] = {
						"Recovered DES key cannot decrypt encrypted content",
						"Encrypted key length or signature length is out of range",
						"Modulus length is out of range",
						"Private key cannot encrypt message digest, or cannot decrypt encrypted key",
						"Public key cannot encrypt data encryption key, or cannot decrypt signature",
						"Signature is incorrect",
						"Unknown Error",
						NULL };

	if(type == 0) {		/* Non RSAEURO Related Error */
		puts(task);			/* Internal Deal with it */
		return;
	}

	/* Convert the type to a string if it is recognized.	 */
	switch(type) {
	case RE_KEY:
		typeString = msg[0];
		break;
	case RE_LEN:
		typeString = msg[1];
		break;
	case RE_MODULUS_LEN:
		typeString = msg[2];
		break;
	case RE_PRIVATE_KEY:
		typeString = msg[3];
		break;
	case RE_PUBLIC_KEY:
		typeString = msg[4];
		break;
	case RE_SIGNATURE:
		typeString = msg[5];
		break;

	default:
		printf("ERROR: Code 0x%04x, %s", type, msg[6]);
		fflush(stdout);
		return;
	}

	printf("ERROR: %s while %s\n", typeString, task);
	fflush(stdout);
}
