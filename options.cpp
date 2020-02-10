#include "stdafx.h"

#include "DoHide.h"
#include "Thread.h"
#include "WinList.h"
#include "dialogs.h"
#include "ProgDlg.h"
#include "hyperl.h"
#include "bitmap.h"
#include "hotkeys.h"
#include "register.h"
#include "defend.h"
#include "globals.h"
#include "rsa.h"
#include "language.h"

#include "resource.h"


#define LVS_EX_CHECKBOXES       0x00000004
#define LVS_EX_FULLROWSELECT    0x00000020 // applies to report mode only
#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)   // optional wParam == mask
#define ListView_SetExtendedListViewStyle(hwndLV, dw)\
        (DWORD)::SendMessage((hwndLV), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw)


const char FMT_STR[]	     = "%d ms.";
const char REGISTER_STR[]    = "register.html";
const char HIDDEN_PASSWORD[] = "         ";

THyperLink Link1;
THyperLink Link2;
THyperLink Link3;
THyperLink Link4;
//THyperLink Link5;
//THyperLink Link6;
TBitmap    Bitmap1(IDB_LOGO);
TBitmap    Bitmap2(IDB_LOGO1);


/*
 *
 *
 */
void UpdateRadioButtons( HWND hDlg )
{
	bool ch13 = (Button_GetCheck(GET(IDC_CHECK13))==BST_CHECKED);

	EnableWindow(GET(IDC_RADIO1),ch13);
	EnableWindow(GET(IDC_RADIO2),ch13);
}





/*
 *
 *
 */
void SetLastPropertyPanel( HWND hDlg, UINT x )
{
	uKey[8] = x;

	HWND hWnd = GetParent(hDlg);
	LONG style = GetWindowLong(hWnd,GWL_EXSTYLE);
	SetWindowLong(hWnd,GWL_EXSTYLE,style&(~WS_EX_CONTEXTHELP));
}


/*
 *
 * 
 */
void SetResponseTime(HWND hDlg)
{
	char s[10];
	int time = SendMessage(GET(IDC_RESPONSE),TBM_GETPOS,0,0);
	wsprintf(s,FMT_STR,time);
	SendMessage(GET(IDC_TIME),WM_SETTEXT,0,(long)s);
}


/*
 *
 *
 */
BOOL CALLBACK PropOptionsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG: 
		{
			SetLastPropertyPanel( hDlg, 1 );

			FillLanguageComboBox ( hDlg, IDC_LANG_COMBO );

			Button_SetCheck(GET(IDC_CHECK1), uKey[9]);
			Button_SetCheck(GET(IDC_CHECK4), uKey[2]);
			Button_SetCheck(GET(IDC_CHECK6), uKey[3]);
			Button_SetCheck(GET(IDC_CHECK8), uKey[5]);
			Button_SetCheck(GET(IDC_CHECK13),uKey[7]);
			Button_SetCheck(GET(IDC_CHECK14),uKey[12]);
			Button_SetCheck(GET(IDC_CHECK15),uKey[13]);
			Button_SetCheck(GET(IDC_CHECK5), !uKey[10]);
			Button_SetCheck(GET(IDC_CHECK11),!uKey[11]);
			Button_SetCheck(GET(IDC_CHECK12),!uKey[14]);
			Button_SetCheck(GET(IDC_CHECK16),!uKey[16]);
			Button_SetCheck(GET(IDC_CHECK17),!uKey[18]);
			Button_SetCheck(GET(IDC_CHECK18),!uKey[19]);
			Button_SetCheck(GET(IDC_CHECK19),!uKey[20]);

			Button_SetCheck(GET(IDC_RADIO1),!uKey[15]);
			Button_SetCheck(GET(IDC_RADIO2),uKey[15]);

			SendMessage(GET(IDC_RESPONSE),TBM_SETRANGE,TRUE,MAKELONG(150, 3000));
			SendMessage(GET(IDC_RESPONSE),TBM_SETTICFREQ,500,0);
			SendMessage(GET(IDC_RESPONSE),TBM_SETPOS,TRUE,uKey[6]);
			SetResponseTime(hDlg);

			if ( Security->ReadPassword() )
			{
				SendMessage( GET(IDC_PASSWORD1), WM_SETTEXT, 0, (LPARAM)HIDDEN_PASSWORD );
				SendMessage( GET(IDC_PASSWORD2), WM_SETTEXT, 0, (LPARAM)HIDDEN_PASSWORD );
			}
			Button_SetCheck(GET(IDC_CHECK100),uKey[17]&PWD_OPTIONS);
			Button_SetCheck(GET(IDC_CHECK101),uKey[17]&PWD_CLOSE);
			Button_SetCheck(GET(IDC_CHECK102),uKey[17]&PWD_SHOW_WINDOW);
			Button_SetCheck(GET(IDC_CHECK103),uKey[17]&PWD_CONTEXT_MENU);
			
			UpdateRadioButtons( hDlg );

			Security->m_bPasswordChanged = false;
		}

		_CASE WM_HSCROLL:
			SetResponseTime(hDlg);

		_CASE WM_COMMAND:
			switch ( GET_WM_COMMAND_ID(wParam, lParam) ) 
			{
				case IDC_CHECK13:
					UpdateRadioButtons( hDlg );
					break;
				case IDC_PASSWORD1:
				case IDC_PASSWORD2:
					if ( HIWORD(wParam)==EN_CHANGE )
						Security->m_bPasswordChanged = true;
					break;
			}

		_CASE WM_NOTIFY:
    		switch (((NMHDR FAR *) lParam)->code) 
    		{
				case PSN_HELP: 
					WinHelp( ID_OPTIONS_TAB );

  				_CASE PSN_KILLACTIVE:
				{
					char s1[128];
					char s2[128];
					HWND hEdit = GET(IDC_PASSWORD1);

					if ( Security->m_bPasswordChanged )
					{
						SendMessage( hEdit,              WM_GETTEXT, (WPARAM)sizeof(s1), (LPARAM)s1 );
						SendMessage( GET(IDC_PASSWORD2), WM_GETTEXT, (WPARAM)sizeof(s2), (LPARAM)s2 );

						if ( lstrcmp(s1,s2) )
						{
							SetFocus(hEdit);
							SendMessage( hEdit, EM_SETSEL, 0, 1000 );

							LoadStr( IDS_PASSWORD_VERIFY, s1, sizeof(s1) );

							MessageBox(hDlg,s1,0,MB_OK|MB_ICONSTOP);
		           			SetWindowLong(hDlg,	DWL_MSGRESULT, TRUE);
							return 1;
						}
						Security->computeHash( s1, Security->m_hash );
					}

					uKey[17] = 0;
					if (Button_GetCheck(GET(IDC_CHECK100))==BST_CHECKED) uKey[17] |= PWD_OPTIONS;
					if (Button_GetCheck(GET(IDC_CHECK101))==BST_CHECKED) uKey[17] |= PWD_CLOSE;
					if (Button_GetCheck(GET(IDC_CHECK102))==BST_CHECKED) uKey[17] |= PWD_SHOW_WINDOW;
					if (Button_GetCheck(GET(IDC_CHECK103))==BST_CHECKED) uKey[17] |= PWD_CONTEXT_MENU;
					
					UINT id  = SendMessage( GET(IDC_LANG_COMBO), CB_GETCURSEL, 0, 0 ); 
					uKey[0]  = SendMessage( GET(IDC_LANG_COMBO), CB_GETITEMDATA, id, 0 ); 
					
					uKey[9]  = (Button_GetCheck(GET(IDC_CHECK1))==BST_CHECKED);
					uKey[2]  = (Button_GetCheck(GET(IDC_CHECK4))==BST_CHECKED);
					uKey[3]  = (Button_GetCheck(GET(IDC_CHECK6))==BST_CHECKED);
					uKey[5]  = (Button_GetCheck(GET(IDC_CHECK8))==BST_CHECKED);
					uKey[6]  = SendMessage(GET(IDC_RESPONSE),TBM_GETPOS,0,0); 
					uKey[7]  = (Button_GetCheck(GET(IDC_CHECK13))==BST_CHECKED);
					uKey[13] = (Button_GetCheck(GET(IDC_CHECK15))==BST_CHECKED);
					uKey[15] = (Button_GetCheck(GET(IDC_RADIO2))==BST_CHECKED);

					uKey[12] = (Button_GetCheck(GET(IDC_CHECK14))==BST_CHECKED);
					TrayMessage(uKey[12]?NIM_DELETE:NIM_ADD);

					uKey[10] = !(Button_GetCheck(GET(IDC_CHECK5))==BST_CHECKED);
					DesktopTrayMessage(uKey[10]?NIM_DELETE:NIM_ADD);

					uKey[11] = !(Button_GetCheck(GET(IDC_CHECK11))==BST_CHECKED);
					RecentTrayMessage(uKey[11]?NIM_DELETE:NIM_ADD);

					uKey[14] = !(Button_GetCheck(GET(IDC_CHECK12))==BST_CHECKED);
					FavoritesTrayMessage(uKey[14]?NIM_DELETE:NIM_ADD);

					uKey[16] = !(Button_GetCheck(GET(IDC_CHECK16))==BST_CHECKED);
					DialupTrayMessage(uKey[16]?NIM_DELETE:NIM_ADD);

					uKey[18] = !(Button_GetCheck(GET(IDC_CHECK17))==BST_CHECKED);
					ControlPanelTrayMessage(uKey[18]?NIM_DELETE:NIM_ADD);
					
					uKey[19] = !(Button_GetCheck(GET(IDC_CHECK18))==BST_CHECKED);
					PrintersTrayMessage(uKey[19]?NIM_DELETE:NIM_ADD);

					uKey[20] = !(Button_GetCheck(GET(IDC_CHECK19))==BST_CHECKED);
					CpuTrayMessage(uKey[20]?NIM_DELETE:NIM_ADD,0);

					StoreConfiguration( Security->m_bPasswordChanged );
					SetupTimer();
	           		SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);

					return 1;
				}

				_DEFAULT:
					return FALSE;
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
BOOL CALLBACK PropHotkeyDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int iCurrItem = 0;

	switch (uMsg)
	{
		case WM_INITDIALOG:
			{
			SetLastPropertyPanel( hDlg, 2 );

			for ( int i=IDS_HOTKEY1; i<=IDS_HOTKEY1+MAX_USED_HOTKEY; i++ ) 
			{
				char s[80];
				LoadStr(i,s,sizeof(s));
				ListBox_AddString(GET(IDC_LIST2), s);
			}
			ListBox_SetCurSel(GET(IDC_LIST2), 0);
			iCurrItem = 0;
			SendMessage(GET(IDC_HOTKEY1), HKM_SETHOTKEY, HotKeys.GetKey(HotKeys.Translate(iCurrItem)), 0);
			Button_SetCheck(GET(IDC_CHECK1),HotKeys.IsAddToMenu(HotKeys.Translate(iCurrItem)) );
			HotKeys.UnregisterAllHotKey();
			}

		_CASE WM_COMMAND:
			if ( GET_WM_COMMAND_ID(wParam, lParam)==IDC_BUTTON1 )
			{
				iCurrItem = ListBox_GetCurSel(GET(IDC_LIST2));

				char HELPFILE_NAME[MAX_PATH];
				WinHelp( (HWND)hDlg, GetHelpFileName(HELPFILE_NAME), HELP_CONTEXTPOPUP, (DWORD)(LPSTR)IDC_HOTKEY1+HotKeys.Translate(iCurrItem) ); 
			}
			else
			if ( HIWORD(wParam)==LBN_SELCHANGE )
			{
				HotKeys.SetKey( HotKeys.Translate(iCurrItem), SendMessage(GET(IDC_HOTKEY1), HKM_GETHOTKEY, 0, 0) );
				HotKeys.SetAddToMenu( HotKeys.Translate(iCurrItem), (Button_GetCheck(GET(IDC_CHECK1))==BST_CHECKED) );
				iCurrItem = ListBox_GetCurSel(GET(IDC_LIST2));
				SendMessage(GET(IDC_HOTKEY1), HKM_SETHOTKEY, HotKeys.GetKey(HotKeys.Translate(iCurrItem)), 0);
				Button_SetCheck(GET(IDC_CHECK1),HotKeys.IsAddToMenu(HotKeys.Translate(iCurrItem)) );
			}
			
		_CASE WM_NOTIFY:
    		switch (((NMHDR FAR *) lParam)->code) 
    		{
				case PSN_HELP: 
					WinHelp( ID_HOTKEY_TAB );

				_CASE PSN_QUERYCANCEL:
					HotKeys.LoadFromRegistry();
					HotKeys.RegisterAllHotKey();
		           	SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
					return 1;

				_CASE PSN_KILLACTIVE:
					HotKeys.SetKey( HotKeys.Translate(iCurrItem), SendMessage(GET(IDC_HOTKEY1), HKM_GETHOTKEY, 0, 0) );
					HotKeys.SetAddToMenu( HotKeys.Translate(iCurrItem), (Button_GetCheck(GET(IDC_CHECK1))==BST_CHECKED) );
					HotKeys.RegisterAllHotKey();
					HotKeys.StoreToRegistry();
	           		SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
					return 1;

				_DEFAULT:
					return FALSE;
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
BOOL CALLBACK PropHideByRunDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			{
				SetLastPropertyPanel( hDlg, 3 );

				char str[40];
				LV_COLUMN column;

				HWND hListWnd = GET(IDC_LIST1);

				LoadStr( IDS_HEADER1, str, sizeof(str) );
				column.mask    = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
				column.fmt     = LVCFMT_LEFT;
				column.cx      = 190;
				column.pszText = str;
				ListView_InsertColumn(hListWnd, 0, &column);

				LoadStr( IDS_HEADER2, str, sizeof(str) );
				column.mask    = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
				column.fmt     = LVCFMT_RIGHT;
				column.cx      = 80;
				column.pszText = str;
				ListView_InsertColumn(hListWnd, 1, &column);

				ListView_SetExtendedListViewStyle(hListWnd,LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

				for ( int i=0; i<MAX_WINDOWS; i++ )
				{	
					THideByRunInfo* info = (THideByRunInfo*)HideTitleList.GetInfo( i );
					if ( info==0 ) 
						continue;
					LV_ITEM item;
					item.mask = LVIF_TEXT;
					item.iItem = i;
					item.iSubItem = 0;

					char szTitles[1024];
					item.pszText = info->UniteTitles(szTitles);
					ListView_InsertItem(hListWnd, &item);
					ListView_SetItemText(hListWnd, i, 1, info->OptionsToString( str ));

					if ( !info->m_bDisabled )
					{
						LV_ITEM item;
						item.mask = LVIF_STATE;
						item.stateMask = LVIS_STATEIMAGEMASK;
						item.state = INDEXTOSTATEIMAGEMASK( LVIS_SELECTED );
						SendMessage(hListWnd, LVM_SETITEMSTATE, i, (long)&item);
					}
				}
			}

		_CASE WM_COMMAND:
			switch ( GET_WM_COMMAND_ID(wParam, lParam) ) 
			{
				case IDC_BTN_ADD:
				{
					char str[20];
					THideByRunInfo* info = new THideByRunInfo;
					HWND hListWnd = GET(IDC_LIST1);
					if ( HideByRunDialog( hDlg, *info )==false )
					{
						delete info;
						break;
					}
					int pos = ListView_GetItemCount( hListWnd );
					LV_ITEM item;
					item.mask = LVIF_TEXT|LVIF_STATE;
					item.iItem = pos;
					item.iSubItem = 0;
					item.stateMask = LVIS_STATEIMAGEMASK;
					item.state = INDEXTOSTATEIMAGEMASK( LVIS_SELECTED );
					char szTitles[1024];
					item.pszText = info->UniteTitles(szTitles);
					ListView_InsertItem( hListWnd, &item );
					ListView_SetItemText( hListWnd, pos, 1, info->OptionsToString( str ) );
					SendMessage(hListWnd, LVM_SETITEMSTATE, pos, (long)&item);
					HideTitleList.Add( info );
				}
				_CASE IDC_BTN_EDIT: 
				{
					char str[20];
					HWND hListWnd = GET(IDC_LIST1);
					int pos = ListView_GetNextItem( hListWnd, -1, LVNI_ALL|LVNI_FOCUSED );
					if ( pos==-1 )
						break;
					THideByRunInfo* info = (THideByRunInfo*)HideTitleList.GetInfo(pos);
					if ( info==0 ) 
						break;
					if ( HideByRunDialog( hDlg, *info )==false )
						break;
					char szTitles[1024];
					ListView_SetItemText( hListWnd, pos, 0, info->UniteTitles(szTitles) );
					ListView_SetItemText( hListWnd, pos, 1, info->OptionsToString( str ) );
				}
				_CASE IDC_BTN_REMOVE: 
				{
					int pos = ListView_GetNextItem( GET(IDC_LIST1), -1, LVNI_ALL|LVNI_FOCUSED );
					if ( pos==-1 )
						break;

					char s1[60];
					LoadStr( IDS_REMOVE, s1, sizeof(s1) );
					if ( MessageBox( hDlg,s1,DOHIDE,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2 )!=IDYES )
						break;
					
					ListView_DeleteItem( GET(IDC_LIST1), pos );
					HideTitleList.Delete( pos );
				}
			}

		_CASE WM_NOTIFY:
    		switch (((NMHDR FAR *) lParam)->code) 
    		{
				case PSN_HELP: 
					WinHelp( ID_HIDEBYRUN_TAB );

				_CASE PSN_QUERYCANCEL:
					HideTitleList.UpdateList();
		           	SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
					return 1;

  				_CASE PSN_KILLACTIVE:
					{
						HWND hListWnd = GET(IDC_LIST1);
						for ( int i=0; i<MAX_WINDOWS; i++ )
						{	
							THideByRunInfo* info = (THideByRunInfo*)HideTitleList.GetInfo( i );
							if ( info==0 ) 
								continue;
		
							int select = SendMessage(hListWnd,LVM_GETITEMSTATE,i,LVIS_STATEIMAGEMASK  );
							info->m_bDisabled = ( select&0x00002000 )?false:true;
						}
					}
					HideTitleList.StoreToRegistry( );
					HideTitleList.UpdateList();
					StoreConfiguration();
		           	SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
					return 1;

				_DEFAULT:
					return FALSE;
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
BOOL CALLBACK WelcomeDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			{
			SetLastPropertyPanel( hDlg, 0 );

			Bitmap1.Init(GET(IDC_BITMAP));
			Bitmap2.Init(GET(IDC_BITMAP1));
			Link1.Init(GET(IDC_HYPERLINK));
			Link2.Init(GET(IDC_HYPERLINK2));
			Link3.Init(GET(IDC_DAYSLEFT));
			Link4.Init(GET(IDC_DAYSLEFT2));
//			Link5.Init(GET(IDC_HYPERLINK5));
//			Link6.Init(GET(IDC_HYPERLINK6));

			SetTimer(hDlg, 1, 1000, 0);

#ifdef NEW_DEFENCE
			char s[256];

			HWND	hWnd	= GET(IDC_DAYSLEFT2);
			WNDPROC wndProc = (WNDPROC)GetWindowLong( hWnd, GWL_WNDPROC );

			CallWindowProc( wndProc, hWnd, WM_GETTEXT, sizeof(s), (WPARAM)s ); 
#ifdef OEM_VERSION
			if ( IS_REGISTRED() )	
				lstrcat( s, Register->m_szUserName );
			else 
				s[0] = 0;
#else
			lstrcat( s, Register->m_szUserName );
#endif
			CallWindowProc( wndProc, hWnd, WM_SETTEXT, 0, (WPARAM)s ); 
#endif
			}

		_CASE WM_TIMER:
#ifdef OEM_VERSION
			if ( !IS_REGISTRED() )	
#endif
			ShowWindow( GET(IDC_DAYSLEFT), SW_SHOWNORMAL );
			KillTimer(hDlg,1);

		_CASE WM_CTLCOLORSTATIC:
			if ( lParam==(long)GET(IDC_HYPERLINK) || 
				 lParam==(long)GET(IDC_HYPERLINK2) ||
				 lParam==(long)GET(IDC_DAYSLEFT) ||
				 lParam==(long)GET(IDC_DAYSLEFT2)||
				 lParam==(long)GET(IDC_HYPERLINK5) ||
				 lParam==(long)GET(IDC_HYPERLINK6) 
			   )
			{
	            THyperLink* link = (THyperLink*)GetWindowLong( (HWND)lParam, GWL_USERDATA ); 
				return link->CtlColor( (HDC)wParam );
			}

		_CASE WM_NOTIFY:
    		switch (((NMHDR FAR *) lParam)->code) 
    		{
				case PSN_HELP: 
					{
					char HELPFILE_NAME[MAX_PATH];
					WinHelp( hMainDlg, GetHelpFileName(HELPFILE_NAME), HELP_INDEX, 0 );
					}

				_CASE PSN_QUERYCANCEL:
  				 case PSN_KILLACTIVE:
		           	SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
					return 1;
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

// Dummy
TEditBox RegName;
TEditBox RegCode;
// Dummy

BOOL CALLBACK RegisterDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			SetLastPropertyPanel( hDlg, 5 );
			Link1.Init(GET(IDC_HYPERLINK));
			Link2.Init(GET(IDC_HYPERLINK2));

			SendMessage( GET(IDC_EDIT3), WM_SETTEXT, (WPARAM)0, (LPARAM)Register->m_szUserName );
			SendMessage( GET(IDC_EDIT4), WM_SETTEXT, (WPARAM)0, (LPARAM)Register->m_szRegCode  );

		_CASE WM_CTLCOLORSTATIC:
			if ( lParam==(long)GET(IDC_HYPERLINK) || lParam==(long)GET(IDC_HYPERLINK2))
			{
	            THyperLink* link = (THyperLink*)GetWindowLong( (HWND)lParam, GWL_USERDATA ); 
				return link->CtlColor( (HDC)wParam );
			}

			// Dummy
			RegName.Init( GET(IDC_EDIT3), Register->m_szUserName );
			RegCode.Init( GET(IDC_EDIT4), Register->m_szRegCode );
			// Dummy

		_CASE WM_COMMAND:
			switch ( GET_WM_COMMAND_ID(wParam, lParam) )
			{
				 case IDC_BUTTON1:
					WinHelp( ID_REGISTER ); 
				_CASE IDC_BUTTON2:
					ShellExecute(hMainDlg, SHELL_OPEN, REGISTER_STR, 0, "", SW_MAXIMIZE );
			}

		_CASE WM_NOTIFY:
    		switch (((NMHDR FAR *) lParam)->code) 
    		{
				case PSN_HELP: 
					WinHelp( ID_REGISTER );

  				_CASE PSN_KILLACTIVE:
					Register->m_szUserName[0] = Register->m_szRegCode[0]  = 0;
					
					SendMessage( GET(IDC_EDIT3), WM_GETTEXT, (WPARAM)sizeof(Register->m_szUserName), (LPARAM)Register->m_szUserName );
					SendMessage( GET(IDC_EDIT4), WM_GETTEXT, (WPARAM)sizeof(Register->m_szRegCode),  (LPARAM)Register->m_szRegCode  );
					
					SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);

#ifdef VER_30_1_DEFENCE
					Sleep(800);
#endif
					return 1;

				_DEFAULT:
					return FALSE;
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
void UpdateProgDialogButtons( HWND hDlg )
{
	int count = ProgramList.GetCount();

	EnableWindow( GET(IDC_BTN_EDIT),   ( count )?TRUE:FALSE );
	EnableWindow( GET(IDC_BTN_REMOVE), ( count )?TRUE:FALSE );

	int pos = ListView_GetNextItem(  GET(IDC_LIST1), -1, LVNI_ALL|LVNI_FOCUSED );
	if ( pos==-1 )
		return;

	EnableWindow( GET(IDC_BTN_UP),   ( pos!=0 )?TRUE:FALSE       );
	EnableWindow( GET(IDC_BTN_DOWN), ( pos!=count-1 )?TRUE:FALSE );
}

/*
 *
 *
 */
BOOL CALLBACK ProgramsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HIMAGELIST hImageList;
	static bool bProgramListChanged;

	switch (uMsg)
	{
		case WM_INITDIALOG:
			{
				bProgramListChanged = false;
				SetLastPropertyPanel( hDlg, 4 );

				char str[128];
				LoadStr( IDS_HEADER3, str, sizeof(str) );

				HWND hListWnd = GET(IDC_LIST1);
				LV_COLUMN column;
				column.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
				column.fmt = LVCFMT_LEFT;
				column.cx = 160;
				column.pszText = str;
				ListView_InsertColumn(hListWnd, 0, &column);

				LoadStr( IDS_HEADER2, str, sizeof(str) );
				column.mask    = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
				column.fmt     = LVCFMT_LEFT;
				column.cx      = 110;
				column.pszText = str;
				ListView_InsertColumn(hListWnd, 1, &column);

				ListView_SetExtendedListViewStyle(hListWnd,LVS_EX_FULLROWSELECT);

				hImageList = ImageList_Create( 16, 16, ILC_COLOR, 0, 1 ); 
				ListView_SetImageList( hListWnd, hImageList, LVSIL_SMALL ); 
				for ( int i=0; i<MAX_WINDOWS; i++ )
				{	
					TProgramInfo* info = (TProgramInfo*)ProgramList.GetInfo( i );
					if ( info==0 ) 
						continue;
					ImageList_AddIcon( hImageList, info->GetFileIcon() );
					LV_ITEM item;
					item.mask = LVIF_TEXT|LVIF_IMAGE;
					item.iItem = i;
					item.iSubItem = 0;
					item.pszText = info->GetTitle();
					item.iImage = i;
					ListView_InsertItem(hListWnd, &item);
					ListView_SetItemText(hListWnd, i, 1, info->m_szCommandLine);
 				}
			}
		
		_CASE WM_DESTROY:
			ImageList_Destroy( hImageList );

		_CASE WM_COMMAND:
			switch ( GET_WM_COMMAND_ID(wParam, lParam) ) 
			{
				case IDC_BTN_ADD:
				{
					TProgramInfo* info = new TProgramInfo;
					HWND hListWnd = GET(IDC_LIST1);
					if ( ProgramToTray( hDlg, *info )==false )
					{
						delete info;
						break;
					}
					int pos = ListView_GetItemCount( hListWnd );
					LV_ITEM item;
					item.mask     = LVIF_TEXT|LVIF_IMAGE;
					item.iItem    = pos;
					item.iSubItem = 0;
					item.pszText  = info->GetTitle();
					item.iImage   = pos;
					ListView_InsertItem( hListWnd, &item );
					ListView_SetItemText(hListWnd, pos, 1, info->m_szCommandLine);
					ProgramList.Add( info );
					ImageList_AddIcon( hImageList, info->GetFileIcon() );
					bProgramListChanged = true;
					
					UpdateProgDialogButtons( hDlg );
				}
				_CASE IDC_BTN_EDIT: 
				{
					HWND hListWnd = GET(IDC_LIST1);
					int pos = ListView_GetNextItem( hListWnd, -1, LVNI_ALL|LVNI_FOCUSED );
					if ( pos==-1 )
						break;
					TProgramInfo* info = ProgramList.GetInfo(pos);
					if ( ProgramToTray( hDlg, *info )==false )
						break;
					ListView_SetItemText( hListWnd, pos, 0, info->GetTitle() );
					ListView_SetItemText( hListWnd, pos, 1, info->m_szCommandLine );
					ImageList_ReplaceIcon( hImageList, pos, info->GetFileIcon() );
					ListView_Update( hListWnd, pos );
					ProgramList.UpdateIcon( info );
					bProgramListChanged = true;
					
					UpdateProgDialogButtons( hDlg );
				}
				_CASE IDC_BTN_REMOVE: 
				{
					int pos = ListView_GetNextItem( GET(IDC_LIST1), -1, LVNI_ALL|LVNI_FOCUSED );
					if ( pos==-1 )
						break;

					char s1[60];
					LoadStr( IDS_REMOVE, s1, sizeof(s1) );
					if ( MessageBox( hDlg,s1,DOHIDE,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2 )!=IDYES )
						break;
					
					ListView_DeleteItem( GET(IDC_LIST1), pos );
					ProgramList.Delete( pos );
					bProgramListChanged = true;
					
					UpdateProgDialogButtons( hDlg );
				}

				_CASE IDC_BTN_UP:
				{
					HWND hListWnd = GET(IDC_LIST1);
					int pos = ListView_GetNextItem( hListWnd, -1, LVNI_ALL|LVNI_FOCUSED );
					if ( pos==-1 || pos==0 )
						break;
					TProgramInfo* info1 = ProgramList.GetInfo(pos-1);
					TProgramInfo* info2 = ProgramList.GetInfo(pos);
					ProgramList.SetInfo(info1, pos);
					ProgramList.SetInfo(info2, pos-1);

					ListView_SetItemText( hListWnd, pos-1, 0, info2->GetTitle() );
					ListView_SetItemText( hListWnd, pos-1, 1, info2->m_szCommandLine );
					ListView_SetItemText( hListWnd, pos,   0, info1->GetTitle() );
					ListView_SetItemText( hListWnd, pos,   1, info1->m_szCommandLine );
					ImageList_ReplaceIcon( hImageList, pos-1, info2->GetFileIcon() );
					ImageList_ReplaceIcon( hImageList, pos,   info1->GetFileIcon() );
					ListView_Update( hListWnd, pos   );
					ListView_Update( hListWnd, pos-1 );
					ListView_SetItemState( hListWnd, pos-1, LVIS_FOCUSED|LVIS_SELECTED, 0x000F );
					bProgramListChanged = true;
				}

				_CASE IDC_BTN_DOWN:
				{
					HWND hListWnd = GET(IDC_LIST1);
					int pos = ListView_GetNextItem( hListWnd, -1, LVNI_ALL|LVNI_FOCUSED );
					if ( pos==-1 || pos==ProgramList.GetCount()-1 )
						break;
					TProgramInfo* info1 = ProgramList.GetInfo(pos+1);
					TProgramInfo* info2 = ProgramList.GetInfo(pos);
					ProgramList.SetInfo(info1, pos);
					ProgramList.SetInfo(info2, pos+1);

					ListView_SetItemText( hListWnd, pos+1, 0, info2->GetTitle() );
					ListView_SetItemText( hListWnd, pos+1, 1, info2->m_szCommandLine );
					ListView_SetItemText( hListWnd, pos,   0, info1->GetTitle() );
					ListView_SetItemText( hListWnd, pos,   1, info1->m_szCommandLine );
					ImageList_ReplaceIcon( hImageList, pos+1, info2->GetFileIcon() );
					ImageList_ReplaceIcon( hImageList, pos,   info1->GetFileIcon() );
					ListView_Update( hListWnd, pos   );
					ListView_Update( hListWnd, pos+1 );
					ListView_SetItemState( hListWnd, pos+1, LVIS_FOCUSED|LVIS_SELECTED, 0x000F );
					bProgramListChanged = true;
				}
			}

		_CASE WM_NOTIFY:
    		switch (((NMHDR FAR *) lParam)->code) 
    		{
				case PSN_HELP: 
					WinHelp( ID_PROGRAM_TAB );

				_CASE PSN_QUERYCANCEL:
					if ( bProgramListChanged )
					{
						ProgramList.RemoveAllIcon();
						ProgramList.UpdateList();
					}
		           	SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
					return 1;

  				_CASE PSN_KILLACTIVE:
					if ( bProgramListChanged )
					{
						ProgramList.StoreToRegistry( );
						ProgramList.RemoveAllIcon();
						ProgramList.UpdateList();
					}
		           	SetWindowLong(hDlg,	DWL_MSGRESULT, FALSE);
					return 1;

				_CASE LVN_ITEMCHANGED:
					UpdateProgDialogButtons( hDlg );

				_DEFAULT:
					return FALSE;
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
void PreparePage ( PROPSHEETPAGE& psp, int templ, int icon, DLGPROC dlgproc, int title)
{
    psp.dwSize		= sizeof(PROPSHEETPAGE);
    psp.dwFlags		= PSP_USEICONID | PSP_USETITLE | PSP_HASHELP;
    psp.hInstance	= hResourceInst;
    psp.pszTemplate	= MAKEINTRESOURCE(templ);
    psp.pszIcon		= MAKEINTRESOURCE(icon);
    psp.pfnDlgProc	= dlgproc;
    psp.pszTitle	= MAKEINTRESOURCE(title);
    psp.lParam		= 0;
    psp.pfnCallback	= NULL; 
}


bool bIsPasswordWindow = false;

BOOL CALLBACK PasswordDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int iPasswordCnt = 0; 

	switch ( uMsg )
	{
		case WM_INITDIALOG:
			bIsPasswordWindow = true;
			iPasswordCnt      = 0;
			SetForegroundWindow(hDlg);

		_CASE WM_DESTROY:
			bIsPasswordWindow = false;

		_CASE WM_COMMAND:
			switch ( GET_WM_COMMAND_ID(wParam, lParam) ) 
			{
				case IDCANCEL:
					EndDialog(hDlg, -1);     

				_CASE IDOK:
				{
					char s[128];
					HWND hEdit = GET(IDC_PASSWORD);

					SendMessage( hEdit, WM_GETTEXT, (WPARAM)sizeof(s), (LPARAM)s );

					BYTE hash[16];
					Security->computeHash( s, hash );

					if ( memcmp(Security->m_hash, hash, sizeof(hash))==0 ) 
					{
						memcpy(Security->m_hash, hash, sizeof(hash));
						EndDialog(hDlg, 1);     
						break;
					}

					SetFocus(hEdit);
					SendMessage( hEdit, EM_SETSEL, 0, 1000 );
					ShowWindow(hDlg,SW_HIDE);
					Sleep(100);
					ShowWindow(hDlg,SW_SHOWNORMAL);
					
					if ( ++iPasswordCnt>=3 )
					{
						EndDialog(hDlg, -1);     
					}
				}
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
bool CheckPassword ( unsigned int uPwdType )
{
	if ( (uKey[17]&uPwdType)==0 )
		return true;	

	if ( Security->ReadPassword()==false )
	{
		return true;	
	}

	if ( bIsPasswordWindow )
		return false;

	if ( DialogBox( hResourceInst, MAKEINTRESOURCE(IDD_PASSWORD_DIALOG), hMainDlg, (DLGPROC)PasswordDlgProc )!=1 )
		return false;

	return true;
}


/*
 *
 *
 */
void CreateOptionsDialog ( int StartPage )
{
	if ( !CheckPassword( PWD_OPTIONS ) )
		return;

	char s[40];
	CreateOptionsDialogTitle( s );

	if ( FindWindow(0,s) )
		return;

    PROPSHEETPAGE psp[6];
    PROPSHEETHEADER psh;

	PreparePage ( psp[0], IDD_WELCOME_DIALOG,  IDI_DOHIDE,   (DLGPROC)WelcomeDlgProc,       IDS_PROPERTIES5 );
	PreparePage ( psp[1], IDD_OPTIONS_DIALOG,  IDI_OPTIONS,  (DLGPROC)PropOptionsDlgProc,   IDS_PROPERTIES1 );
	PreparePage ( psp[2], IDD_HOTKEY_DIALOG,   IDI_HOTKEY,   (DLGPROC)PropHotkeyDlgProc,    IDS_PROPERTIES2 );
	PreparePage ( psp[3], IDD_LOCK_DIALOG,     IDI_LOCK,     (DLGPROC)PropHideByRunDlgProc, IDS_PROPERTIES3 );
	PreparePage ( psp[4], IDD_PROGRAMS_DIALOG, IDI_PROGRAM,  (DLGPROC)ProgramsDlgProc,      IDS_PROPERTIES6 );
	PreparePage ( psp[5], IDD_REGISTER_DIALOG, IDI_REGISTER, (DLGPROC)RegisterDlgProc,      IDS_PROPERTIES4 );
	
    psh.dwSize		= sizeof(PROPSHEETHEADER);
    psh.dwFlags		= PSH_PROPSHEETPAGE | PSH_HASHELP | PSH_NOAPPLYNOW;
	psh.hIcon		= 0;
    psh.hwndParent	= hMainDlg;
	psh.hInstance	= hInst;
    psh.nPages		= sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage	= (StartPage==-1)?uKey[8]:StartPage;
    psh.ppsp		= (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback	= 0;
    psh.pszCaption  = s;


	UINT lng = uKey[0];

    PropertySheet(&psh);

	if ( lng!=uKey[0] )
	{
		bRestart = true;
		SendMessage( hMainDlg, WM_COMMAND, ID_CLOSE_DOHIDE, 0 );
	}
}
