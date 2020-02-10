
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

#ifdef GENKEY_PROJECT
#include <string.h>
#include <math.h>
#include <stdio.h>
__cdecl void main ( int argc, char** argv ) 
{
	if ( argc<2 )
		return;

	unsigned long crcTable[256];
	crcgen( crcTable );

	char s[1024];
	s[0] = 0; 
	
	for ( int i=1; i<argc; i++ )
	{
		strcat(s,argv[i]);
	}
	unsigned long crc = getcrc( crcTable, s, strlen(s) );
	
	printf("User name = %s\n",s);
	printf("Registration code = %d\n",abs(crc));
}
#endif