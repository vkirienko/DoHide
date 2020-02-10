#ifndef __WINLIST__
#define __WINLIST__

#include "DoHideH\\DoHideH.h"

#define MAX_WINDOWS  20

/*
 *
 *
 */
class THideByRunInfo
{
public:
	THideByRunInfo();

	HICON GetFileIcon( );
	void  SplitTitles( char* szTitle );
	char* UniteTitles( char* szTitle );

	void  StringToOptions( char* s );
	char* OptionsToString( char* s );
	char* OptionsToString2( char* s );

	void  StoreToRegistry( int i );
	void  LoadFromRegistry( int i );
	bool  Empty( )          { return m_szTitle[0][0]==0; }
	char* GetTitle( int i ) { return m_szTitle[i]; } 
	int   GetTitleCount( )  { return m_iTitleCount; } 

	bool m_bDisabled;

	char m_szTitle[5][MAX_PATH];
	int  m_iTitleCount;
	bool m_bFilePath;
	bool m_bWindowClass;

	bool m_bHideToToolbar;
	bool m_bHideIfMinimized;
	bool m_bHideAlwaysIfMinimized;
	bool m_bHideAndIfMinimized;
	bool m_bHideByFullTitle;
	bool m_bHideCaseInsensitive;
	bool m_bCloseWindow;
	bool m_bKillProcess;
	bool m_bBeep;
	bool m_bShowWindow;
	bool m_bMoveToOutside;
	bool m_bPressButton;
	bool m_bPermanentIcon;
	bool m_bDeleteTrayIcon;
	bool m_bMaximizeWindow;

	bool m_bHideFirstTime;
	int  m_iHideCounter;

	char m_szIconFile[MAX_PATH];
	int  m_iIcon;

	char m_szPressButtonTitle[25];
};


/*
 *
 *
 */
class TProgramInfo
{
public:
	TProgramInfo();

	bool  Empty( ) { return m_szDescription[0]==0; }
	char* GetTitle( ) { return m_szDescription; } 
	char* GetTitleForTray( char* s );
	void  StoreToRegistry( int i );
	void  LoadFromRegistry( int i );

	HICON GetFileIcon( );

	char m_szDescription[60];
	char m_szFilePath[MAX_PATH];
	char m_szCommandLine[MAX_PATH];
	char m_szStartDir[MAX_PATH];
	int  m_iIcon;
	int  m_iRunOnStartup;
	int  m_iNoIconMark;
};



/* 
 *
 *
 */
class TProcessList
{
public:
	TProcessList ();

	void		AddProcess( WND_INFO* wInfo );
	WND_INFO*	FindWindowInfo( HWND hWnd );
	bool		WindowTitleExist(  HWND hWnd, char* s );

private:
	WND_INFO	m_WndInfo[MAX_WINDOWS];
};


/* 
 *
 *
 */
class THideByRunList
{
public:
	THideByRunList ();
	virtual ~THideByRunList();

	void	FreeList ();
	void	UpdateList ();
	void	StoreToRegistry( );

	void	Add( THideByRunInfo* info );
	void	Delete( int i );

	int		GetCount() { return m_iCount; }
	int		GetDeleteFromTrayCount() { return m_iDeleteFromTrayCount; }
	THideByRunInfo*	GetInfo( int i ) { return m_HideInfo[i]; } 

private:
	THideByRunInfo* m_HideInfo[MAX_WINDOWS];
	int				m_iCount;
	int             m_iDeleteFromTrayCount;
};



/* 
 *
 *
 */
class TProgramList
{
public:
	TProgramList ();
	virtual ~TProgramList();

	void	FreeList ();
	void	UpdateList ();
	void	StoreToRegistry( );

	void	Add( TProgramInfo* info );
	void	Delete( int i );
	void	RemoveAllIcon();
	void	UpdateIcon( TProgramInfo* info );
	void    PTrayMessage(DWORD dwMessage, TProgramInfo* info);

	void    RMouseButtonDown(TProgramInfo* info);
	void    LMouseButtonDown(TProgramInfo* info);
	void	Command( int ID );

	int		GetCount() { return m_iCount; }
	TProgramInfo* GetInfo( int i ) { return m_ProgInfo[i]; } 
	void	SetInfo( TProgramInfo* info, int i ) { m_ProgInfo[i] = info; } 

private:
	TProgramInfo* m_ProgInfo[MAX_WINDOWS];
	int			  m_iCount;
	TProgramInfo* m_iLastProgramInfo;
};


/* 
 *
 *
 */
class TTrayMinimizedWindows
{
public:
	TTrayMinimizedWindows ();
	virtual ~TTrayMinimizedWindows();

	void	Add( HWND hWnd );
	void	Delete( HWND hWnd );
	void	CheckWindows();

private:
	HWND	m_hWnd[MAX_WINDOWS];
	int		m_iCount;
};



extern TProcessList				ProcessList;
extern THideByRunList			HideTitleList;
extern TProgramList				ProgramList;
extern TTrayMinimizedWindows	TrayMinimizedList;

#endif