#ifndef __SHELL_H__
#define __SHELL_H__

#include <shlobj.h>
#include <objbase.h>

extern void CreateStartupLink( LPCSTR lpszPathObj, LPCSTR lpszDesc, LPSTR lpszArguments = "", LPSTR lpszWorkingDirectory = "");
extern void DeleteStartupLink( LPCSTR lpszDesc );

/*
 *
 *
 */
class TShellLink  
{
public:
	TShellLink(); 
	~TShellLink();

	void ShellFillMenu(HMENU hMenu, int iFillType, LPITEMIDLIST pidl = 0);
	void ShellFillRasConnectionMenu(HMENU hMenu, int iMenuItem);
};


extern TShellLink ShellLink;

#endif