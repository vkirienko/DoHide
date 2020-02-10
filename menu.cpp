#include "stdafx.h"

#include "menu.h"
#include "dohide.h"
#include "globals.h"
#include "ras.h"
#include "register.h"
#include "rsa.h"
#include "shellapi.h"

#include "resource.h"

const char DOTS[] = " ...";


CCustomMenu CustomMenu;




/*

#define NCOLORSHADES 128		// this many shades in gradient

void PaintRect(HDC hDC, int x, int y, int w, int h, COLORREF color)
{
	HBRUSH hBrush    = CreateSolidBrush(color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);
	PatBlt(hDC, x, y, w, h, PATCOPY);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);
}

void PaintGradiantRect(HDC hDC, const RECT &rect, COLORREF clrFrom, COLORREF clrTo, BOOL hori, BOOL ascend)
{
	int cxCap = rect.right - rect.left;
	int cyCap = rect.bottom - rect.top;

	// Get the intensity values for the ending color
	int r1 = GetRValue(clrTo); // red
	int g1 = GetGValue(clrTo); // green
	int b1 = GetBValue(clrTo); // blue
	
	// Get the intensity values for the begining color
	int r2 = GetRValue(clrFrom); // red
	int g2 = GetGValue(clrFrom); // green
	int b2 = GetBValue(clrFrom); // blue

	int r, g, b;

    if(hori) //paint horizontal rect;
    {
	    int x = cxCap;	
	    int w = x;							// width of area to shade
	    int xDelta= max(w/NCOLORSHADES,1);	// width of one shade band


	    while (x >= xDelta) {
		    x -= xDelta;
		    if (r1 > r2)
			    r = r1 - (r1-r2)*(w-x)/w;
		    else
			    r = r1 + (r2-r1)*(w-x)/w;

		    if (g1 > g2)
			    g = g1 - (g1-g2)*(w-x)/w;
		    else
			    g = g1 + (g2-g1)*(w-x)/w;

		    if (b1 > b2)
			    b = b1 - (b1-b2)*(w-x)/w;
		    else
			    b = b1 + (b2-b1)*(w-x)/w;

            if(ascend) // Paint from  left to right;
		        PaintRect(hDC, rect.left+x, rect.top, xDelta, cyCap, RGB(r, g, b));
            else               // Paint from  right to left;
                PaintRect(hDC, rect.right-x-xDelta, rect.top, xDelta, cyCap, RGB(r, g, b));
	    }
    }
    else    //paint vertical rect;
    {
	    int y = cyCap;	
	    int w = y;							// height of area to shade
	    int yDelta= max(w/NCOLORSHADES,1);	// height of one shade band


	    //while (y >= yDelta) {
        while (y > 0) {
		    y -= yDelta;
		    if (r1 > r2)
			    r = r1 - (r1-r2)*(w-y)/w;
		    else
			    r = r1 + (r2-r1)*(w-y)/w;

		    if (g1 > g2)
			    g = g1 - (g1-g2)*(w-y)/w;
		    else
			    g = g1 + (g2-g1)*(w-y)/w;

		    if (b1 > b2)
			    b = b1 - (b1-b2)*(w-y)/w;
		    else
			    b = b1 + (b2-b1)*(w-y)/w;
		    
            if(ascend) // Paint from  top to bottom;
		        PaintRect(hDC, rect.left, rect.top+y, cxCap, yDelta, RGB(r, g, b));
            else       // Paint from  bottom to top;
                PaintRect(hDC, rect.left, rect.bottom-y-yDelta, cxCap, yDelta, RGB(r, g, b));
	    }
    }

}



int ySize = 0;
int ySize1 = 0;

*/




CCustomMenu::CCustomMenu () : hFont(0)
{
}


CCustomMenu::~CCustomMenu ()
{
	FreeVector ();
}


/*
 *
 *
 */
void CCustomMenu::Initialize (HMENU hmenu, int MenuType )
{
	FreeVector ();

//	ySize = 0;
//	ySize1 = 0;

	hMenu      = hmenu;
	m_MenuType = MenuType;
	
	m_iCurrMenuItem = -1;
	m_hCurrSubMenu  = 0;

	/* 
	 *	Create menu font
	 */
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, (PVOID)&ncm, 0 );
	hFont = CreateFontIndirect(&ncm.lfMenuFont);
}


/*
 *
 *
 */
void CCustomMenu::FreeVector ()
{
	int size = items.size();

	for ( int i=0; i<size; i++ )
	{
		switch ( types[i] )
		{
			case MIT_PIDL: 
				GetMalloc()->Free( (void*)items[i] );
				break;

			case MIT_DIALUP_NAME:
				delete (char*)items[i];
				break;
		}
	}

	items.clear();
	types.clear();

	/* 
	 *	Delete menu font
	 */
	if (hFont)
		DeleteObject(hFont);

	hFont = 0;

	CoFreeUnusedLibraries();
}


/*
 *
 *
 */
int CCustomMenu::AddItem ( int item, CMenuItemType type )
{
	items.push_back( item );
	types.push_back( type );

	return items.size()-1;
}


/*
 *
 *
 */
void CCustomMenu::OnMeasureItem( HWND hwnd, LPMEASUREITEMSTRUCT lpmis )
{
	int x_size = GetSystemMetrics( SM_CXSCREEN )/4;

    HDC hdc = GetDC(hwnd); 
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	char s[128];
	GetItemInfo( lpmis->itemData, s );
	
    SIZE size; 
    GetTextExtentPoint32(hdc, s, lstrlen(s), &size); 

	lpmis->itemWidth  = ((size.cx>x_size) ? x_size : size.cx) + 35; 
    lpmis->itemHeight = GetSystemMetrics(SM_CYMENUSIZE); 
 
//	ySize += lpmis->itemHeight;

	SelectObject(hdc,hOldFont);
    ReleaseDC(hwnd, hdc); 

	CHECK_REGISTRATION_2( 702, CLEAR_COUNTER, 41, ACT_REMOVE_PROGRAM_ICONS );
}


/*
 *
 *
 */
//extern HWND hSupportWindow;
//HDC hSrcDC; 
void CCustomMenu::OnDrawItem( LPDRAWITEMSTRUCT lpdis )
{
/*	if ( ySize1==0 )
	{
		hSrcDC = CreateCompatibleDC( lpdis->hDC );
		HBITMAP hSrcBitmap = CreateCompatibleBitmap(lpdis->hDC,16,ySize+150);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject( hSrcDC, hSrcBitmap );
		//hSrcDC = GetWindowDC(hSupportWindow); 
		//hSrcDC = GetWindowDC(0); 
		RECT r;
		r.bottom = ySize+150;
		r.top    = 0;
		r.left	 = 0;
		r.right  = 16;
		PaintGradiantRect(hSrcDC, r, RGB(0,0,255), RGB(0,0,100), false, true);
		ySize1 = 1;
	}
*/
	HFONT hOldFont = (HFONT)SelectObject(lpdis->hDC,hFont);

    COLORREF clrPrevText, clrPrevBkgnd; 
 
    // Set the appropriate foreground and background colors. 
    if (lpdis->itemState & ODS_SELECTED) 
    { 
        clrPrevText  = SetTextColor(lpdis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT)); 
        clrPrevBkgnd = SetBkColor(lpdis->hDC, GetSysColor(COLOR_HIGHLIGHT)); 
    } 
    else 
    { 
        clrPrevText  = SetTextColor(lpdis->hDC, GetSysColor(COLOR_MENUTEXT)); 
        clrPrevBkgnd = SetBkColor(lpdis->hDC, GetSysColor(COLOR_MENU)); 
    } 
 
    // Determine where to draw and leave space for a check-mark. 
	LONG dim = GetMenuCheckMarkDimensions();
	int  h   = lpdis->rcItem.bottom-lpdis->rcItem.top;
    int  x   = lpdis->rcItem.left+((m_MenuType==-1)?LOWORD(dim):5); 
//    int  x   = lpdis->rcItem.left+((m_MenuType==-1)?LOWORD(dim):5)+16; 
    int  y   = lpdis->rcItem.top; 

//	PRINTF("x=%d   rcItem.left=%d   m_MenuType=%d ", x, lpdis->rcItem.left, m_MenuType);

	char s[128];
	HICON hIcon;
	GetItemInfo( lpdis->itemData, s, &hIcon );

	int x_size = GetSystemMetrics( SM_CXSCREEN )/4 - 27;
	int len    = lstrlen(s);

    SIZE size; 
    GetTextExtentPoint32(lpdis->hDC, s, len, &size); 
	while ( size.cx>x_size && len )
	{
		s[--len] = 0;
		GetTextExtentPoint32(lpdis->hDC, s, len, &size); 
		if ( size.cx<=x_size )
		{
			lstrcat( s, DOTS );
			len += 4;
		}
	}

    ExtTextOut(lpdis->hDC, x+27, y+(h-size.cy)/2+1, ETO_OPAQUE, &lpdis->rcItem, s, len, NULL); 

	// Draw icon
	if ( hIcon )
		DrawIconEx(lpdis->hDC, x, y+(h-16)/2+1,hIcon,16,16,0,0,DI_NORMAL);

	// Draw check-mark
	if ( lpdis->itemState & ODS_CHECKED )
	{
		HDC     hMemDC = CreateCompatibleDC( lpdis->hDC );
		HBITMAP hBmp   = LoadBitmap(0,MAKEINTRESOURCE(32760));
		HBITMAP hOldBitmap = (HBITMAP)SelectObject( hMemDC, hBmp );
		BitBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top+(h-HIWORD(dim))/2, LOWORD(dim), HIWORD(dim), hMemDC, 0, 0, SRCCOPY );
		SelectObject(hMemDC, hOldBitmap );
		DeleteObject(hBmp);
		DeleteDC(hMemDC);
	}

//	BitBlt( lpdis->hDC, 0, y, 15, h, hSrcDC, 0, y, SRCCOPY );

    // Restore the original font and colors. 
    SetTextColor(lpdis->hDC, clrPrevText); 
    SetBkColor(lpdis->hDC, clrPrevBkgnd); 
	SelectObject(lpdis->hDC,hOldFont);
}

 
/*
 *
 *
 */
void CCustomMenu::OnExecute( int ID )
{
	switch ( types[ID] )
	{
		case MIT_HWND: 
		{
			HWND hWnd = (HWND)items[ID];
			if ( IsWindowVisible(hWnd) ) HideOneWindow(hWnd,uKey[5]); 
			else ShowOneWindow( hWnd, true );
			break;
		}
		
		case MIT_DIALUP_NAME:
			ras.Dial( (char*)items[ID] );
			break;

		case MIT_PIDL: 
		{
			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei,sizeof(SHELLEXECUTEINFO));

			sei.cbSize   = sizeof(SHELLEXECUTEINFO);
			sei.nShow    = SW_SHOWNORMAL;
			sei.fMask    = SEE_MASK_INVOKEIDLIST; 
			sei.lpIDList = (void*)items[ID];

			ShellExecuteEx(&sei);
			break;
		}
	}

	FreeVector();
}


/*
 *
 *
 */
void CCustomMenu::GetItemInfo( int ID, char* szItemName, HICON* phIcon )
{
	LPITEMIDLIST pidl;

	switch ( types[ID] )
	{
		case MIT_HWND: 
			GetWndText( (HWND)items[ID], szItemName, 128 );
			ReplaceDesktopIcons( szItemName );
			break;
		
		case MIT_DIALUP_NAME:
			lstrcpy(szItemName,(char*)items[ID]);
			break;
		
		case MIT_PIDL: 
			GetShellItemName( pidl = (LPITEMIDLIST)items[ID], SHGDN_NORMAL, szItemName );
			break;
	}

	if ( phIcon )
	{
		switch ( types[ID] )
		{
			case MIT_HWND: 
				*phIcon = GetWindowIcon( (HWND)items[ID] );
				break;
			
			case MIT_DIALUP_NAME:
				*phIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_DIAL));
				break;
			
			case MIT_PIDL: 
			{
				SHFILEINFO  sfi;
				sfi.dwAttributes = SFGAO_DISPLAYATTRMASK;

				if( !::SHGetFileInfo((LPCTSTR)pidl, NULL, &sfi, sizeof(sfi),
					SHGFI_PIDL|SHGFI_ATTRIBUTES|SHGFI_DISPLAYNAME) )
				{
					*phIcon = 0;
					break;
				}
				
				BOOL bIsLink = sfi.dwAttributes & SFGAO_LINK;
				SHGetFileInfo((LPCTSTR)pidl, NULL, &sfi, sizeof(sfi),
						SHGFI_PIDL|SHGFI_ICON|SHGFI_SMALLICON|
						(bIsLink?SHGFI_LINKOVERLAY:NULL));
				
				*phIcon = sfi.hIcon;
				break;
			}
		}
	}
}


/*
 *
 *
 */
void CCustomMenu::Sort()
{
	char s1[128];
	char s2[128];

	int size = items.size();

	bool sorted = false;
	while ( !sorted ) 
	{
		sorted = true; 
		for (int j=0; j<size-1; j++) 
		{
			GetItemInfo( j, s1 );
			GetItemInfo( j+1, s2 );
			if ( lstrcmp(s1,s2)>0 ) 
			{
				int id = items[j];
				items[j] = items[j+1];
				items[j+1] = id;
				sorted = false;
			}
		}
	}
}



/*
 *
 *
 */
LRESULT CCustomMenu::OnMenuChar( char chPressedChar, HMENU hmenu )
{
	if ( hmenu!=m_hCurrSubMenu )
	{
		m_iCurrMenuItem = -1;
	}

	char s[128];
	int count = GetMenuItemCount(hmenu);

	CharUpperBuff( &chPressedChar, 1 );

	if ( m_iCurrMenuItem!=-1 )
	{
		SpecGetItemText( hmenu, m_iCurrMenuItem, s );

		if ( s[0]==chPressedChar && m_iCurrMenuItem+1<count )
		{
			SpecGetItemText( hmenu, m_iCurrMenuItem+1, s );

			if ( s[0]==chPressedChar )
			{
				m_hCurrSubMenu = hmenu;
				m_iCurrMenuItem++;
				return MAKELRESULT(m_iCurrMenuItem,MNC_SELECT);
			}
		}
	}

	for ( int i=0; i<count; i++ ) 
	{
		SpecGetItemText( hmenu, i, s );

		if ( s[0]==chPressedChar )
		{
			m_hCurrSubMenu = hmenu;
			m_iCurrMenuItem = i;
			return MAKELRESULT(m_iCurrMenuItem,MNC_SELECT);
		}
	}

	return MAKELRESULT(0,MNC_IGNORE);
}


/*
 *
 *
 */
void CCustomMenu::SpecGetItemText( HMENU hmenu, int ID, char* str )
{
	MENUITEMINFO mi;
	ZeroMemory( &mi, sizeof(mi) );
	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask  = MIIM_DATA | MIIM_TYPE;

	GetMenuItemInfo( hmenu, ID, TRUE, &mi );

	if ( mi.fType&MFT_OWNERDRAW )
	{
		GetItemInfo( mi.dwItemData, str );
		CharUpperBuff( &str[0], 1 );
		return;
	}

	str[0] = 0;
}
