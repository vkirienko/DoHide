#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "winlist.h"


#define GET( x ) GetDlgItem(hDlg,(x))

extern void  InitFileNameStruct( OPENFILENAME& ofn );
extern HICON GetFileIcon( char*, int );
extern HICON GetWindowIcon( HWND hWnd );
extern char* LoadStr( int ID, char* s, int size );
extern char* CvtString( char* s );
extern void  ShowMenu( HMENU hMenu );
extern void  FreeMenu ( HMENU hMenu );
extern void  WinHelp( int ID );
extern char* GetHelpFileName( char* );

extern void  GetWndText( HWND hWnd, char* s, int str_len );
extern int   GetWndTextLen( HWND hWnd );

extern bool isWinNT;
extern bool isWinNT2K;
extern bool isWinXP;

extern bool  CompareTitle ( char* szTitle1, char* szTitle2, bool bCaseInsensitive, bool bFullTitle );
extern bool  CompareTitle ( THideByRunInfo* info, char* buf );

extern void  CreateOptionsDialogTitle( char* s );
extern void  RegisterAsService( int iRegister );

extern bool IsWindowVisibleOnScreen ( HWND hWnd );

extern void CreateSupportWindow();
extern void DestroySupportWindow();

#endif