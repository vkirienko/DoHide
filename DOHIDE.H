#ifndef __DOHIDE_H__
#define __DOHIDE_H__

/*
 *	Global defenitions
 *
 */
#define NEW_DEFENCE
#define VER_30_1_DEFENCE

//#define OEM_VERSION
//#define OEM_NAME_E_AND_S


/*
 *	
 *
 */
#define DOHIDE_VERSION	33
#define COUNT_KEYS		21

#define WM_NOTIFYICON			(WM_APP+100)
#define WM_NOTIFYICON_DESKTOP	(WM_NOTIFYICON+2)
#define WM_NOTIFYICON_RECENT	(WM_NOTIFYICON+3)
#define WM_NOTIFYICON_FAVORITES	(WM_NOTIFYICON+4)
#define WM_NOTIFYICON_DIALUP	(WM_NOTIFYICON+5)
#define WM_NOTIFYICON_CONTROL	(WM_NOTIFYICON+6)
#define WM_NOTIFYICON_PRINTERS	(WM_NOTIFYICON+7)
#define WM_NOTIFYICON_CPU		(WM_NOTIFYICON+8)

#define WM_TRAY_PROGRAM_NOTIFY	(WM_APP+200)
#define WM_HOTKEY_COMMAND		(WM_APP+300)
#define WM_EXECUTE_CUSTOM_MENU	(WM_APP+400)
	
#define WM_UNREGISTRED			(WM_APP+1000)
#define WM_CHECK_REGISTRATION	(WM_APP+1001)

#define ID_CLOSE_DOHIDE			3000

#define HKID_SHOW_ALL				0
#define HKID_HIDE_ALL				1
#define HKID_HIDE_ALL_TOOLBAR		2
#define HKID_HIDE_CURR				3
#define HKID_HIDE_CURR_TOOLBAR		4
#define HKID_HIDE_AGAIN				5
#define HKID_MINIMIZE_ALL			6
#define HKID_MINIMIZE_CURRENT		7
#define HKID_RESTORE_ALL			8
#define HKID_MAXIMIZE_CURRENT		9
#define HKID_RUN_SCREEN_SAVER		10
#define HKID_LOGOFF					11
#define HKID_SHUTDOWN				12
#define HKID_REBOOT					13
#define HKID_KILL_PROCESS			14
#define HKID_CONTROL_PANEL 			15
#define HKID_DISPLAY_PROPERTIES		16
#define HKID_DESKTOP_SHORTCUTS		17
#define HKID_DOCUMENTS				18
#define HKID_SET_WINDOW_TOPMOST		19
#define HKID_MAXIMIZE_X				20
#define HKID_MAXIMIZE_Y				21
#define HKID_MOVE_TOP				22
#define HKID_MOVE_BOTTOM			23
#define HKID_MOVE_LEFT				24
#define HKID_MOVE_RIGHT				25
#define HKID_CLOSE_ALL_WINDOWS		26
#define HKID_OPEN_CD_TRAY			27
#define HKID_CLOSE_CD_TRAY			28
#define HKID_FAVORITES				29
#define HKID_DOHIDE_ICON			30
#define HKID_DIALUP_NETWORKING		31
#define HKID_HARD_SHUTDOWN			32
#define HKID_HARD_REBOOT			33
#define HKID_MAXIMIZE_ALL			34
#define MAX_USED_HOTKEY				HKID_MAXIMIZE_ALL

extern HINSTANCE hInst;
extern HINSTANCE hResourceInst;

extern const char DOHIDE[]; 
extern const char DOHIDE_PROP[];
extern const char DOHIDE_PROP1[];
extern const char DOHIDE_PROP2[];
extern HANDLE HIDDEN_BY_DOHIDE1; 
extern const char SHELL_OPEN[];
extern const char SECTION_DOHIDE[];
extern const char _REGSTR_PATH_RUN[];
extern const char SZ_PASSWORD[];


extern void ShowOneWindow ( HWND hWnd, bool bShowConsoleWindow = false, bool bRemoveTrayIcon = false );
extern void HideOneWindow ( HWND hWnd, BOOL bHideToToolbar=FALSE, BOOL bNeedRestore=FALSE, HICON hIcon=0, bool bPermanentTrayIcon = false );
extern bool UseWindow     ( HWND hWnd, bool bIgnoreTitleLength=false );
extern void StoreConfiguration( bool bSavePassword=false );
extern void KillProcess( HWND hWnd );
extern void SetupTimer();
extern void ReplaceDesktopIcons( char* s );
extern void TrayMessage(DWORD dwMessage, PSTR pszTip="");
extern void DesktopTrayMessage(DWORD dwMessage);
extern void RecentTrayMessage(DWORD dwMessage);
extern void FavoritesTrayMessage(DWORD dwMessage);
extern void DialupTrayMessage(DWORD dwMessage);
extern void ControlPanelTrayMessage(DWORD dwMessage);
extern void PrintersTrayMessage(DWORD dwMessage);
extern void CpuTrayMessage(DWORD dwMessage, int iCpuUsage);
extern void ProcessDDEAndCmdLine ( char* s );
extern void CommandMessage (WPARAM wParam, LPARAM lParam);


extern HWND  hMainDlg;
extern UINT  uKey[COUNT_KEYS];
extern bool  bRestart;

#ifdef NEW_DEFENCE
extern DWORD dwAppStartTime;
extern DWORD dwAppCurrTime;

//	Check registration after 2 hour 11 minutes
#define IsTimeToCheckRegistration()	 ( (dwAppCurrTime-dwAppStartTime)>7860000 )  
#endif


#define _CASE               break; case 
#define _DEFAULT            break; default 


#define CSIDL_DIALUP		0x91893911

#define PWD_NO_PASSWORD		0
#define PWD_OPTIONS			1
#define PWD_CLOSE			2
#define PWD_SHOW_WINDOW		4
#define PWD_CONTEXT_MENU	8

#endif