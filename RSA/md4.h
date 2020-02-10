/* 	 MD4 Message Digest Algorithm.
*/

#ifndef _MD4_H_
#define _MD4_H_

#ifdef __cplusplus
extern "C" {
#endif

/* MD4 context. */
typedef struct
{
	UINT4 state[4];                                   /* state (ABCD) */
	UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD4_CTX;

void MD4Init PROTO_LIST ((MD4_CTX *));
void MD4Update PROTO_LIST ((MD4_CTX *, unsigned char *, unsigned int));
void MD4Final PROTO_LIST ((unsigned char [16], MD4_CTX *));

#ifdef __cplusplus
}
#endif

#endif /* _MD4_H_ */
