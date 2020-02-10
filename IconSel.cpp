#include "stdafx.h"

#include "DoHide.h"
#include "bitmap.h"
#include "WinList.h"

#include "resource.h"

TBitmap ProgIcon(IDC_PROGRAM_ICON,false);

/*
 *
 *
 */
void InitIcon( HWND hWnd )
{
	ProgIcon.Init(hWnd);
}


/*
 *
 *
 */
void SetupIcon( HWND hWnd, char* IconFile, int IconId )
{
	int IconCount = (int)ExtractIcon(hInst,IconFile,-1);
	SendMessage(hWnd,TBM_SETRANGE,TRUE,MAKELONG(0, IconCount-1));
	SendMessage(hWnd,TBM_SETTICFREQ,1,0);
	SendMessage(hWnd,TBM_SETPOS,TRUE,IconId);
}


/*
 *
 *
 */
void UpdateIcon( HWND hSlider, HWND hIconWnd, HWND hIconFileWnd )
{
	TProgramInfo info;
	info.m_iIcon = SendMessage(hSlider,TBM_GETPOS,0,0);
	Edit_GetText(hIconFileWnd, info.m_szFilePath, sizeof(info.m_szFilePath) );
	ProgIcon.SetId( (int)info.GetFileIcon() );
	InvalidateRect( hIconWnd,0,TRUE );
}

