#include "stdafx.h"

#include "DoHide.h"
#include "globals.h"
#include "hyperl.h"

#include "resource.h"


const char SUBJECT[] = 	"?subject="; 



/*
 *	Subclass procedure 
 *
 */
LRESULT APIENTRY HyperLinkSubclassProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
    THyperLink* link = (THyperLink*)GetWindowLong( hwnd, GWL_USERDATA ); 
    switch ( uMsg ) 
	{
		case WM_SETCURSOR:         
 			link->SetCursor();
			return TRUE;

		case WM_MOUSEMOVE:
			link->MouseMove( hwnd, LOWORD(lParam), HIWORD(lParam) );
			return TRUE;
			
		case WM_LBUTTONDOWN:
			link->Execute( hwnd );
			return TRUE;
	}
    
	return CallWindowProc((WNDPROC)link->wpOrigHyperLinkProc, hwnd, uMsg, wParam, lParam); 
} 

/*
 *
 *
 */
void THyperLink::Init( HWND hWnd )
{
	m_State = ST_NOT_VISITED;
	m_hLinkCursor = LoadCursor(hInst,MAKEINTRESOURCE(IDC_HLINK));
    wpOrigHyperLinkProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)HyperLinkSubclassProc); 
    SetWindowLong(hWnd, GWL_USERDATA, (LONG)this); 
}


/*
 *
 *
 */
void THyperLink::HighLight(HWND hwnd, BOOL state)
{
	if (state)
	{
		if (m_State != ST_HIGHLIGHTED)
		{
			m_OldState = m_State;
			m_State = ST_HIGHLIGHTED;
			InvalidateRect(hwnd, NULL, TRUE);
		}
	}
	else
	{
		if (m_State == ST_HIGHLIGHTED)
		{
			m_State = m_OldState;
			InvalidateRect(hwnd, NULL, TRUE);
		}
	}
}


/*
 *
 *
 */
int THyperLink::CtlColor(HDC hDC)
{
	switch (m_State)
	{
	   case ST_NOT_VISITED:	SetTextColor(hDC,RGB(0, 0, 255)); break;
	   case ST_VISITED:	  	SetTextColor(hDC,RGB(128, 0, 128)); break;
	   case ST_HIGHLIGHTED:	SetTextColor(hDC,RGB(255, 0, 0)); break;
	}
	SetBkMode(hDC,TRANSPARENT);

	// return hollow brush to preserve parent background color
	return (int)::GetStockObject(HOLLOW_BRUSH);
}


/*
 *
 *
 */
void THyperLink::MouseMove( HWND hwnd, int x, int y )
{
	POINT point;
	RECT rc;
	GetClientRect(hwnd,&rc);
	point.x = x;
	point.y = y;
	if (PtInRect(&rc,point))
	{
		if (m_State != ST_HIGHLIGHTED)
		{
			SetCapture(hwnd);
			HighLight(hwnd,TRUE);
		} 
	}
	else
	{
		if (m_State == ST_HIGHLIGHTED)
		{
			HighLight(hwnd,FALSE);
			ReleaseCapture();
		}
	}
}

/*
 *
 *
 */
void THyperLink::Execute(HWND hwnd)
{
	char s[1024];

	GetWndText( hwnd, s, sizeof(s) );

	if ( s[0]=='m' )
	{
		char title[40];

		CreateOptionsDialogTitle( title );
		lstrcat( s, SUBJECT ); 
		lstrcat( s, title ); 
	}

	ShellExecute(hMainDlg, SHELL_OPEN, s, 0, "", SW_MAXIMIZE );
	m_State = ST_VISITED;
}



// Dummy

/*
 *	Subclass procedure 
 *
 */
LRESULT APIENTRY EditBoxSubclassProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	TEditBox* link = (TEditBox*)GetWindowLong( hwnd, GWL_USERDATA ); 

    if ( uMsg==WM_DESTROY )         
	{
		CallWindowProc( (WNDPROC)link->m_wpOrigEditBoxProc, hwnd, WM_GETTEXT, 80, (WPARAM)link->m_szEditStr ); 
		PRINTF(link->m_szEditStr);
		//link->m_szEditStr = 0;
	}
    
	return CallWindowProc((WNDPROC)link->m_wpOrigEditBoxProc, hwnd, uMsg, wParam, lParam); 
} 


/*
 *
 *
 */
TEditBox::TEditBox()
{
	m_szEditStr = 0;
}

/*
 *
 *
 */
void TEditBox::Init( HWND hWnd, char* s )
{
	if ( m_szEditStr )
		return;

	m_szEditStr		    = s;
    m_wpOrigEditBoxProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)EditBoxSubclassProc); 

    SetWindowLong(hWnd, GWL_USERDATA, (LONG)this); 
}

// Dummy
