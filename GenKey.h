#ifndef __GENKEY_H__
#define __GENKEY_H__

extern void crcgen( unsigned long* crcTable );
extern unsigned long getcrc( unsigned long* crcTable, char* s, int size );

#endif