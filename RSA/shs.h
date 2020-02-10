/*
	SHS.H - header file for Secure Hash Standard Code

	Secure Hash Standard Code header file.
*/

#ifndef _SHS_H_
#define _SHS_H_

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The SHS block size and message digest sizes, in bytes */

#define SHS_BLOCKSIZE   64
#define SHS_DIGESTSIZE  20

/* The structure for storing SHS info */

typedef struct
{
	UINT4 digest [5];             /* Message digest */
	UINT4 countLo, countHi;       /* 64-bit bit count */
	UINT4 data [16];              /* SHS data buffer */
} SHS_CTX;

void SHSInit PROTO_LIST ((SHS_CTX *));
void SHSUpdate PROTO_LIST ((SHS_CTX *, unsigned char *, int ));
void SHSFinal PROTO_LIST ((unsigned char *, SHS_CTX *));

#ifdef __cplusplus
}
#endif

#endif /* _SHS_H_ */
