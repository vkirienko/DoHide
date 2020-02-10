#include "stdafx.h"

#include "DoHide.h"
#include "IconSel.h"
#include "Globals.h"
#include "WinList.h"
#include "bitmap.h"

#include "resource.h"

#define GET_CHECKBOX(ID) ( Button_GetCheck(GET(ID))==BST_CHECKED && IsWindowEnabled(GET(ID)) )

THideByRunInfo* HideInfo;
TBitmap TargetBitmap(0,false); 


/*
 *
 *
 */
void ShowOptionsDialog( UINT uShow )
{
	char s[40];
	CreateOptionsDialogTitle( s ); 
	HWND hWnd = FindWindow( 0, s );
	if ( hWnd )
		ShowWindow( hWnd, uShow );
}


/*
 *
 *
 */
bool Enable( HWND hDlg, int exclude, bool enable )
{
	if ( exclude!=IDC_CHECK22 )
		EnableWindow(GET(IDC_CHECK22),enable);
	if ( exclude!=IDC_CHECK23 )
		EnableWindow(GET(IDC_CHECK23),enable);
	if ( exclude!=IDC_CHECK28 )
		EnableWindow(GET(IDC_CHECK28),enable);

	return !enable;
}


/*
 *
 *
 */
bool Enable2( HWND hDlg, int exclude, bool enable )
{
	if ( exclude!=IDC_CHECK20 )
	{
		EnableWindow(GET(IDC_CHECK20),enable);
		EnableWindow(GET(IDC_CHECK15),enable);
		EnableWindow(GET(IDC_EDIT3),enable);
		EnableWindow(GET(IDC_BUTTON1),enable);
		EnableWindow(GET(IDC_SLIDER1),enable);
	}
	
	if ( exclude!=IDC_CHECK24 )
		EnableWindow(GET(IDC_CHECK24),enable);
	
	if ( exclude!=IDC_CHECK25 )
		EnableWindow(GET(IDC_CHECK25),enable);
	
	if ( exclude!=IDC_CHECK29 )
		EnableWindow(GET(IDC_CHECK29),enable);
	
	if ( exclude!=IDC_CHECK30 )
	{
		EnableWindow(GET(IDC_CHECK30),enable);
		EnableWindow(GET(IDC_EDIT25),enable);
	}
	
	if ( exclude!=IDC_CHECK31 )
		EnableWindow(GET(IDC_CHECK31),enable);
	
	if ( exclude!=IDC_CHECK33 )
	{
		EnableWindow(GET(IDC_CHECK33),enable);
	}

	if ( exclude!=IDC_CHECK35 )
		EnableWindow(GET(IDC_CHECK35),enable);

	return !enable;
}



/*
 *
 *
 */
void UpdateCheckBoxes( HWND hDlg )
{
	bool ch22 = (Button_GetCheck(GET(IDC_CHECK22))==BST_CHECKED);
	bool ch23 = (Button_GetCheck(GET(IDC_CHECK23))==BST_CHECKED);
	bool ch28 = (Button_GetCheck(GET(IDC_CHECK28))==BST_CHECKED);

	if ( Enable( hDlg, IDC_CHECK22, !ch22 ) ) return;
	if ( Enable( hDlg, IDC_CHECK23, !ch23 ) ) return;
	if ( Enable( hDlg, IDC_CHECK28, !ch28 ) ) return;
}


/*
 *
 *
 */
void UpdateCheckBoxes2( HWND hDlg )
{
	bool ch20 = (Button_GetCheck(GET(IDC_CHECK20))==BST_CHECKED);
	bool ch24 = (Button_GetCheck(GET(IDC_CHECK24))==BST_CHECKED);
	bool ch25 = (Button_GetCheck(GET(IDC_CHECK25))==BST_CHECKED);
	bool ch29 = (Button_GetCheck(GET(IDC_CHECK29))==BST_CHECKED);
	bool ch30 = (Button_GetCheck(GET(IDC_CHECK30))==BST_CHECKED);
	bool ch31 = (Button_GetCheck(GET(IDC_CHECK31))==BST_CHECKED);
	bool ch33 = (Button_GetCheck(GET(IDC_CHECK33))==BST_CHECKED);
	bool ch35 = (Button_GetCheck(GET(IDC_CHECK35))==BST_CHECKED);

	if ( Enable2( hDlg, IDC_CHECK20, !ch20 ) ) return;
	if ( Enable2( hDlg, IDC_CHECK24, !ch24 ) ) return;
	if ( Enable2( hDlg, IDC_CHECK25, !ch25 ) ) return;
	if ( Enable2( hDlg, IDC_CHECK29, !ch29 ) ) return;
	if ( Enable2( hDlg, IDC_CHECK30, !ch30 ) ) return;
	if ( Enable2( hDlg, IDC_CHECK31, !ch31 ) ) return;
	if ( Enable2( hDlg, IDC_CHECK33, !ch33 ) ) return;
	if ( Enable2( hDlg, IDC_CHECK35, !ch35 ) ) return;
}


/*
 *
 *
 */
/*void DrawFrame(HWND hWnd)///!!!!!!!!!!!!!!!!
{
	if ( hWnd==(HWND)-1 ) 
		return;

	RECT rect;
	GetWindowRect(hWnd,&rect);
	HDC hDC = GetWindowDC(hWnd);
	
	HBRUSH hBrush    = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN   hPen      = CreatePen(PS_SOLID,4,RGB(255,255,255));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);
	HPEN   hOldPen   = (HPEN)SelectObject(hDC,hPen);
	SetROP2(hDC,R2_XORPEN);
	Rectangle(hDC,0,0,rect.right-rect.left,rect.bottom-rect.top);
	SelectObject(hDC,hOldPen);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	ReleaseDC(hWnd,hDC);
}
*/

/*
 *
 *
 */
BOOL CALLBACK HideByRunDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HICON hTargetIcon;
	static HICON hTargetIcon1;
	static bool  bMouseCapture = false;
	static HWND  hCurrWnd = (HWND)-1;
	static RECT  rcDialogRect;

	switch ( uMsg )
	{
		case WM_INITDIALOG:
			InitIcon(GET(IDC_PROGRAM_ICON));

			TargetBitmap.Init(GET(IDC_TARGET_BITMAP));
			hTargetIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TARGET));
			hTargetIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TARGET1));
			TargetBitmap.SetId( (int)hTargetIcon );

			char szTitles[1024];

			int  id;
			if ( HideInfo->m_bFilePath ) id = IDC_EDIT21;
			else if ( HideInfo->m_bWindowClass ) id = IDC_EDIT24;
			else id = IDC_EDIT20;

			Edit_SetText( GET(id), HideInfo->UniteTitles(szTitles) );
			Button_SetCheck(GET(IDC_RADIO2),  !(HideInfo->m_bFilePath || HideInfo->m_bWindowClass) );
			Button_SetCheck(GET(IDC_RADIO3),  HideInfo->m_bFilePath );
			Button_SetCheck(GET(IDC_RADIO4),  HideInfo->m_bWindowClass );
			Button_SetCheck(GET(IDC_CHECK20), HideInfo->m_bHideToToolbar );
			Button_SetCheck(GET(IDC_CHECK22), HideInfo->m_bHideIfMinimized );
			Button_SetCheck(GET(IDC_CHECK23), HideInfo->m_bHideAlwaysIfMinimized );
			Button_SetCheck(GET(IDC_CHECK28), HideInfo->m_bHideAndIfMinimized );
			Button_SetCheck(GET(IDC_CHECK21), HideInfo->m_bHideByFullTitle );
			Button_SetCheck(GET(IDC_CHECK24), HideInfo->m_bCloseWindow );
			Button_SetCheck(GET(IDC_CHECK25), HideInfo->m_bKillProcess );
			Button_SetCheck(GET(IDC_CHECK26), HideInfo->m_bBeep );
			Button_SetCheck(GET(IDC_CHECK27), HideInfo->m_bHideCaseInsensitive );
			Button_SetCheck(GET(IDC_CHECK29), HideInfo->m_bMoveToOutside );
			Button_SetCheck(GET(IDC_CHECK30), HideInfo->m_bPressButton );
			Button_SetCheck(GET(IDC_CHECK31), !(HideInfo->m_bPressButton | HideInfo->m_bMoveToOutside | HideInfo->m_bKillProcess | HideInfo->m_bCloseWindow | HideInfo->m_bHideToToolbar | HideInfo->m_bDeleteTrayIcon | HideInfo->m_bMaximizeWindow) );
			Button_SetCheck(GET(IDC_CHECK15), HideInfo->m_bPermanentIcon );
			Button_SetCheck(GET(IDC_CHECK33), HideInfo->m_bDeleteTrayIcon );
			Button_SetCheck(GET(IDC_CHECK34), HideInfo->m_bHideFirstTime );
			Button_SetCheck(GET(IDC_CHECK35), HideInfo->m_bMaximizeWindow );

			Edit_SetText(GET(IDC_EDIT3),  HideInfo->m_szIconFile );
			Edit_SetText(GET(IDC_EDIT25), HideInfo->m_szPressButtonTitle );
			SetupIcon( GET(IDC_SLIDER1),  HideInfo->m_szIconFile, HideInfo->m_iIcon );
			UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT3) );
			DragAcceptFiles(hDlg,TRUE);

			UpdateCheckBoxes( hDlg );
			UpdateCheckBoxes2( hDlg );

			ShowOptionsDialog( SW_HIDE );
/*
{
	typedef BOOL (*__SetLayeredWindowAttributes)( HWND, COLORREF, BYTE, DWORD );


    HMODULE hDLL = LoadLibrary("user32.dll");
	FARPROC fp__SetLayeredWindowAttributes = GetProcAddress( hDLL, "SetLayeredWindowAttributes" );

	/*
BOOL SetLayeredWindowAttributes(
  HWND hwnd,           // handle to the layered window
  COLORREF crKey,      // specifies the color key
  BYTE bAlpha,         // value for the blend function
  DWORD dwFlags        // action
);
*/

// Set WS_EX_LAYERED on this window 
//SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | 0x0080000L/*WS_EX_LAYERED*/);
// Make this window 70% alpha
//(*(__SetLayeredWindowAttributes)fp__SetLayeredWindowAttributes)(hDlg, RGB(192,192,192), 0, /*LWA_ALPHA*/1);
//			}

		_CASE WM_DESTROY:
			DragAcceptFiles(hDlg,FALSE);

			DestroyIcon(hTargetIcon);
			DestroyIcon(hTargetIcon1);
			if ( bMouseCapture )
			{
				//DrawFrame(hCurrWnd);	
				ReleaseCapture();
				bMouseCapture = false;
			}

		_CASE WM_LBUTTONDOWN:
			if ( !bMouseCapture )
			{
				POINT point;
				GetCursorPos(&point);
				ScreenToClient(hDlg,&point);
				HWND hWnd = ChildWindowFromPoint(hDlg,point);
				if ( hWnd!=GET(IDC_TARGET_BITMAP) )
					break;

				SetCursor( LoadCursor(hInst, MAKEINTRESOURCE(IDC_TARGET)) );
				SetCapture( hDlg );
				bMouseCapture = true;
				hCurrWnd = (HWND)-1;
				TargetBitmap.SetId( (int)hTargetIcon1 );

				GetWindowRect( hDlg, &rcDialogRect );
				SetWindowPos( hDlg, 0, 0, GetSystemMetrics(SM_CYSCREEN)-155, 0, 0, SWP_NOZORDER|SWP_NOSIZE );
			}

		_CASE WM_LBUTTONUP:
			if ( bMouseCapture )
			{
				//DrawFrame(hCurrWnd);	
				ReleaseCapture();
				bMouseCapture = false;
				TargetBitmap.SetId( (int)hTargetIcon );

				SetWindowPos( hDlg, 0, rcDialogRect.left, rcDialogRect.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE );
			}
			
		_CASE WM_MOUSEMOVE:
			if ( bMouseCapture )
			{
				SetCursor( LoadCursor(hInst, MAKEINTRESOURCE(IDC_TARGET)) );
				POINT point;
				GetCursorPos(&point);
				HWND hWnd = WindowFromPoint(point);
				HWND owner = GetParent(hWnd);
				
				if ( hCurrWnd!=hWnd && hWnd!=hDlg && hWnd!=hMainDlg && owner!=hDlg /*&& UseWindow(hWnd)*/ ) 
				{
					//DrawFrame(hCurrWnd);	
					hCurrWnd = hWnd;

					char s[128];
					GetWndText(hCurrWnd,s,sizeof(s));
					Edit_SetText(GET(IDC_EDIT20), s );
					GetClassName(hCurrWnd,s,sizeof(s));
					Edit_SetText(GET(IDC_EDIT24), s );

					//DrawFrame(hCurrWnd);	
				}
			}

		_CASE WM_DROPFILES:
		{
			HANDLE Handle = (HANDLE)wParam;
			int totalNumberOfFiles = DragQueryFile((HDROP)Handle, -1, 0, 0);

			for ( int i=0; i<totalNumberOfFiles; i++ )
			{
				int  fileLength = DragQueryFile((HDROP)Handle,i,0,0) + 1;
				DragQueryFile((HDROP)Handle, i, HideInfo->m_szTitle[0], fileLength);
				Edit_SetText(GET(IDC_EDIT20), "" );
				Edit_SetText(GET(IDC_EDIT21), HideInfo->m_szTitle[0] );
				HideInfo->m_bFilePath = true;
				HideInfo->m_bWindowClass = false;
				Button_SetCheck(GET(IDC_RADIO2), !(HideInfo->m_bFilePath || HideInfo->m_bWindowClass) );
				Button_SetCheck(GET(IDC_RADIO3), HideInfo->m_bFilePath );
				Button_SetCheck(GET(IDC_RADIO4), HideInfo->m_bWindowClass );
			}

			SetupIcon( GET(IDC_SLIDER1), "", 0 );
			UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT3) );
			SetForegroundWindow(hDlg);	
			BringWindowToTop(hDlg);
			DragFinish((HDROP)Handle);
		}

		_CASE WM_HSCROLL:
			UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT3) );

		_CASE WM_COMMAND:
			switch ( GET_WM_COMMAND_ID(wParam, lParam) ) 
			{
				case IDCANCEL:
					ShowOptionsDialog( SW_SHOWNORMAL );
					EndDialog(hDlg, -1);     

				_CASE IDOK:
				{
					int title   = Button_GetCheck(GET(IDC_RADIO2));
					int name	= Button_GetCheck(GET(IDC_RADIO3));
					int w_class = Button_GetCheck(GET(IDC_RADIO4));

					if ( title==BST_CHECKED ) 
					{
						char szTitles[1024];
						Edit_GetText(GET(IDC_EDIT20), szTitles, 128 );
						HideInfo->SplitTitles(szTitles);
						HideInfo->m_bFilePath = HideInfo->m_bWindowClass = false;
					}
					
					if ( name==BST_CHECKED ) 
					{
						Edit_GetText(GET(IDC_EDIT21), HideInfo->m_szTitle[0], 128 );
						HideInfo->m_bFilePath = true;
						HideInfo->m_bWindowClass = false;
					}

					if ( w_class==BST_CHECKED ) 
					{
						Edit_GetText(GET(IDC_EDIT24), HideInfo->m_szTitle[0], 128 );
						HideInfo->m_bFilePath = false;
						HideInfo->m_bWindowClass = true;
					}

					HideInfo->m_bHideToToolbar			= GET_CHECKBOX(IDC_CHECK20);
					HideInfo->m_bHideIfMinimized		= GET_CHECKBOX(IDC_CHECK22);
					HideInfo->m_bHideAlwaysIfMinimized	= GET_CHECKBOX(IDC_CHECK23);
					HideInfo->m_bHideAndIfMinimized		= GET_CHECKBOX(IDC_CHECK28);
					HideInfo->m_bHideByFullTitle		= GET_CHECKBOX(IDC_CHECK21);
					HideInfo->m_bCloseWindow			= GET_CHECKBOX(IDC_CHECK24);
					HideInfo->m_bKillProcess			= GET_CHECKBOX(IDC_CHECK25);
					HideInfo->m_bBeep					= GET_CHECKBOX(IDC_CHECK26);
					HideInfo->m_bHideCaseInsensitive	= GET_CHECKBOX(IDC_CHECK27);
					HideInfo->m_bMoveToOutside			= GET_CHECKBOX(IDC_CHECK29);
					HideInfo->m_bPressButton			= GET_CHECKBOX(IDC_CHECK30);
					HideInfo->m_bPermanentIcon			= GET_CHECKBOX(IDC_CHECK15);
					HideInfo->m_bDeleteTrayIcon			= GET_CHECKBOX(IDC_CHECK33);
					HideInfo->m_bHideFirstTime			= GET_CHECKBOX(IDC_CHECK34);
					HideInfo->m_bMaximizeWindow			= GET_CHECKBOX(IDC_CHECK35);

					Edit_GetText(GET(IDC_EDIT3),  HideInfo->m_szIconFile, 128 );
					Edit_GetText(GET(IDC_EDIT25), HideInfo->m_szPressButtonTitle, sizeof(HideInfo->m_szPressButtonTitle) );
					HideInfo->m_iIcon = SendMessage(GET(IDC_SLIDER1),TBM_GETPOS,0,0);

					ShowOptionsDialog( SW_SHOWNORMAL );
					EndDialog(hDlg, 1);     
				}

				_CASE IDHELP: 
					WinHelp( ID_HIDE_BY_RUN ); 

				_CASE IDC_BUTTON20:
				{
					char  szName[256] = "";
					char szTemp[128];
					CvtString( LoadStr( IDS_OPFD_2, szTemp, sizeof(szTemp) ) );

					OPENFILENAME ofn;
					InitFileNameStruct(ofn);

					ofn.hwndOwner   = hDlg; // An invalid hWnd causes non-modality
					ofn.lpstrFilter = (LPSTR)szTemp;  // See previous note concerning string
					ofn.lpstrFile   = (LPSTR)szName;  // Stores the result in this variable
					ofn.nMaxFile    = sizeof( szName );
					
					if ( GetOpenFileName( &ofn )==0 )
						break;

					Button_SetCheck(GET(IDC_RADIO2),  0 );
					Button_SetCheck(GET(IDC_RADIO3),  1 );
					Edit_SetText(GET(IDC_EDIT21), ofn.lpstrFile );
				}					

				_CASE IDC_BUTTON1:
				{
					char szName[256] = "";
					char szTemp[128];
					CvtString( LoadStr( IDS_OPFD_1, szTemp, sizeof(szTemp) ) );

					OPENFILENAME ofn;
					InitFileNameStruct(ofn);

					ofn.hwndOwner   = hDlg; // An invalid hWnd causes non-modality
					ofn.lpstrFilter = (LPSTR)szTemp;  // See previous note concerning string
					ofn.lpstrFile   = (LPSTR)szName;  // Stores the result in this variable
					ofn.nMaxFile    = sizeof( szName );
					
					if ( GetOpenFileName( &ofn )==0 )
						break;

					Edit_SetText(GET(IDC_EDIT3), ofn.lpstrFile );
					SetupIcon( GET(IDC_SLIDER1), ofn.lpstrFile, 0 );
					UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT3) );
				}					

				_CASE IDC_EDIT3:
				{
					char s[MAX_PATH];
					Edit_GetText(GET(IDC_EDIT3), s, sizeof(s) );
					SetupIcon( GET(IDC_SLIDER1), s, 0 );
					UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT3) );
				}
	
				_CASE IDC_CHECK28:
				 case IDC_CHECK22:
				 case IDC_CHECK23:
					UpdateCheckBoxes( hDlg );
				
				_CASE IDC_CHECK31:
				 case IDC_CHECK30:
				 case IDC_CHECK29:
				 case IDC_CHECK25:
				 case IDC_CHECK24:
				 case IDC_CHECK20:
				 case IDC_EDIT25:
				 case IDC_CHECK33:
				 case IDC_CHECK35:
					UpdateCheckBoxes2( hDlg );
			}

		_DEFAULT:
			return FALSE;
	}

	return TRUE;
}


/*
 *
 *
 */
bool HideByRunDialog( HWND hWnd, THideByRunInfo& info )
{
	HideInfo = &info;

	if ( DialogBox( hResourceInst, MAKEINTRESOURCE(IDD_HIDE_BY_RUN), hWnd, (DLGPROC)HideByRunDlgProc )!=1 )
		return false;

	if ( info.Empty() )
		return false;

	return true; 
}
