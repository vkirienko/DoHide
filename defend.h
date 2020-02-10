#ifndef __DEFEND_H__
#define __DEFEND_H__

typedef VOID (*__GetSystemTime)( LPSYSTEMTIME );
 


class CLibrary 
{
public:
	CLibrary ();
	~CLibrary();

	__GetSystemTime	fp_GetSystemTime;

private:
	HMODULE hKernel;
};


extern CLibrary* lib;


#endif