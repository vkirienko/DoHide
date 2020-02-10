/* R_STDLIB.C - platform-specific C library routines */

#include "rsaeuro.h"

void R_memset(output, value, len)
POINTER output;                 /* output block */
int value;                      /* value */
unsigned int len;               /* length of block */
{
	if(len != 0)
    {
		do
        {
			*output++ = (unsigned char)value;
		}while(--len != 0);
	}
}

/* Secure memcpy routine */

void R_memcpy(output, input, len)
POINTER output;                 /* output block */
POINTER input;                  /* input block */
unsigned int len;               /* length of blocks */
{
	if (len != 0)
    {
		do
        {
			*output++ = *input++;
		}while (--len != 0);
	}
}

/* Secure memcmp routine */

int R_memcmp(Block1, Block2, len)
POINTER Block1;                 /* first block */
POINTER Block2;                 /* second block */
unsigned int len;               /* length of blocks */
{
	if(len != 0)
    {
		/* little trick in declaring vars */
		register const unsigned char *p1 = Block1, *p2 = Block2;

		do
        {
			if(*p1++ != *p2++)
				return(*--p1 - *--p2);
		}while(--len != 0);
	}
	return(0);
}
