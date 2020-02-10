/*   MD2 Message Digest Algorithm.
*/

#ifndef _MD2_H_
#define _MD2_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  unsigned char state[16];                                 /* state */
  unsigned char checksum[16];                           /* checksum */
  unsigned int count;                 /* number of bytes, modulo 16 */
  unsigned char buffer[16];                         /* input buffer */
} MD2_CTX;

void MD2Init PROTO_LIST ((MD2_CTX *));
void MD2Update PROTO_LIST
  ((MD2_CTX *, unsigned char *, unsigned int));
void MD2Final PROTO_LIST ((unsigned char [16], MD2_CTX *));

#ifdef __cplusplus
}
#endif

#endif /* _MD4_H_ */
