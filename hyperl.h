#ifndef __HYPERL_H__
#define __HYPERL_H__


/*
 *
 *
 */
class THyperLink
{
public:

	THyperLink( ) {}

	void Init( HWND hWnd );
	void SetCursor() { ::SetCursor(m_hLinkCursor); }

	enum State
	{
		ST_NOT_VISITED,
		ST_VISITED,
		ST_HIGHLIGHTED
	};

	WNDPROC wpOrigHyperLinkProc;
	HCURSOR m_hLinkCursor;
	State   m_State;
	State   m_OldState;

	void MouseMove(HWND hwnd, int x, int y );
	void HighLight(HWND hwnd, BOOL state);
	void Execute(HWND hwnd);
	int  CtlColor(HDC hDC);
};


/*
 *
 *
 */
class TEditBox
{
public:
	TEditBox();
	void Init( HWND hWnd, char* s );

	WNDPROC	m_wpOrigEditBoxProc;
	char*	m_szEditStr;
};



#endif