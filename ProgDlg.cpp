#include "stdafx.h"

#include "DoHide.h"
#include "WinList.h"
#include "bitmap.h"
#include "ProgDlg.h"
#include "IconSel.h"
#include "Globals.h"

#include "resource.h"

TProgramInfo* ProgInfo;

/*
 *
 *
 */
BOOL CALLBACK ProgramDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
		case WM_INITDIALOG:
			Edit_SetText(GET(IDC_EDIT20), ProgInfo->m_szDescription );
			Edit_SetText(GET(IDC_EDIT21), ProgInfo->m_szFilePath );
			Edit_SetText(GET(IDC_EDIT22), ProgInfo->m_szCommandLine );
			Edit_SetText(GET(IDC_EDIT23), ProgInfo->m_szStartDir );
			
			Button_SetCheck(GET(IDC_CHECK1), ProgInfo->m_iRunOnStartup );
			Button_SetCheck(GET(IDC_CHECK2), ProgInfo->m_iNoIconMark );

			InitIcon(GET(IDC_PROGRAM_ICON));
			SetupIcon( GET(IDC_SLIDER1), ProgInfo->m_szFilePath, ProgInfo->m_iIcon );
			UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT21) );
			DragAcceptFiles(hDlg,TRUE);

		_CASE WM_DESTROY:
			DragAcceptFiles(hDlg,FALSE);

		_CASE WM_DROPFILES:
		{
			HANDLE Handle = (HANDLE)wParam;
			int totalNumberOfFiles = DragQueryFile((HDROP)Handle, -1, 0, 0);

			for ( int i=0; i<totalNumberOfFiles; i++ )
			{
				int  fileLength = DragQueryFile((HDROP)Handle,i,0,0) + 1;
				DragQueryFile((HDROP)Handle, i, ProgInfo->m_szFilePath, fileLength);
				Edit_SetText(GET(IDC_EDIT21), ProgInfo->m_szFilePath );
			}

			SetupIcon( hDlg, ProgInfo->m_szFilePath, 0 );
			UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT21) );

			SetForegroundWindow(hDlg);	
			BringWindowToTop(hDlg);
			DragFinish((HDROP)Handle);
		}

		_CASE WM_HSCROLL:
			UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT21) );
			
		_CASE WM_COMMAND:
			switch ( GET_WM_COMMAND_ID(wParam, lParam) ) 
			{
				case IDCANCEL:
					EndDialog(hDlg, -1);     

				_CASE IDOK:
					Edit_GetText(GET(IDC_EDIT21), ProgInfo->m_szFilePath, sizeof(ProgInfo->m_szFilePath) );
					Edit_GetText(GET(IDC_EDIT20), ProgInfo->m_szDescription, sizeof(ProgInfo->m_szDescription) );
					Edit_GetText(GET(IDC_EDIT22), ProgInfo->m_szCommandLine, sizeof(ProgInfo->m_szCommandLine) );
					Edit_GetText(GET(IDC_EDIT23), ProgInfo->m_szStartDir, sizeof(ProgInfo->m_szStartDir) );
					ProgInfo->m_iIcon         = SendMessage(GET(IDC_SLIDER1),TBM_GETPOS,0,0);
					ProgInfo->m_iRunOnStartup = (Button_GetCheck(GET(IDC_CHECK1))==BST_CHECKED);
					ProgInfo->m_iNoIconMark   = (Button_GetCheck(GET(IDC_CHECK2))==BST_CHECKED);
					EndDialog(hDlg, 1);     
				
				_CASE IDHELP: 
					WinHelp( ID_ADD_TO_TRAY );

				_CASE ID_BUTTON20:
				{
					char szName[256] = "";
					char szTemp[128];
					CvtString( LoadStr( IDS_OPFD_3, szTemp, sizeof(szTemp) ) );

					OPENFILENAME ofn;
					InitFileNameStruct(ofn);

					ofn.hwndOwner   = hDlg; // An invalid hWnd causes non-modality
					ofn.lpstrFilter = (LPSTR)szTemp;  // See previous note concerning string
					ofn.lpstrFile   = (LPSTR)szName;  // Stores the result in this variable
					ofn.nMaxFile    = sizeof( szName );
					
					if ( GetOpenFileName( &ofn )==0 )
						break;

					Edit_SetText(GET(IDC_EDIT21), ofn.lpstrFile );

					SetupIcon( GET(IDC_SLIDER1), ofn.lpstrFile, 0 );
					UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT21) );
				}					

				_CASE IDC_EDIT21: 
				{
					char s[MAX_PATH];
					Edit_GetText(GET(IDC_EDIT21), s, sizeof(s) );
					SetupIcon( GET(IDC_SLIDER1), s, 0 );
					UpdateIcon( GET(IDC_SLIDER1), GET(IDC_PROGRAM_ICON), GET(IDC_EDIT21) );
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
bool ProgramToTray( HWND hWnd, TProgramInfo& pi )
{
	ProgInfo = &pi;

	if ( DialogBox( hResourceInst, MAKEINTRESOURCE(IDD_TRAYNOT_PROGRAMS), hWnd, (DLGPROC)ProgramDlgProc )!=1 )
		return false;

	if ( pi.m_szFilePath[0]==0 )
		return false;

	return true;  
}

