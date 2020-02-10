#include "StdAfx.h"

#include "DoHide.h"
#include "bitmap.h"
#include "register.h"

#include "resource.h"



/*
 *	Subclass procedure 
 *
 */
LRESULT APIENTRY BitmapSubclassProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
    TBitmap* bitmap = (TBitmap*)GetWindowLong( hwnd, GWL_USERDATA ); 
    switch ( uMsg ) 
	{
		case WM_PAINT:         
 			bitmap->Paint(hwnd);
			return TRUE;
	}
    
	return CallWindowProc((WNDPROC)bitmap->wpOrigBitmapProc, hwnd, uMsg, wParam, lParam); 
} 

/*
 *
 *
 */
void TBitmap::Init( HWND hWnd )
{
    wpOrigBitmapProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)BitmapSubclassProc); 
    SetWindowLong(hWnd, GWL_USERDATA, (LONG)this); 
}


/*
 *
 *
 */
void TBitmap::Paint( HWND hWnd )
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd,&ps);

	if ( m_bBitmap )
	{
		HBITMAP hBmp = LoadBitmap( hInst, MAKEINTRESOURCE(m_ID) );
		DrawTransparent( hBmp, ps.hdc, 0, 0, RGB(198,198,198) );
		DeleteObject(hBmp);
	}
	else
	{
		RECT r;
		GetClientRect(hWnd,&r);
		FillRect(ps.hdc,&r,(HBRUSH)(COLOR_MENU+1));	
		DrawIcon( ps.hdc, 0,0, (HICON)m_ID );
	}
	
	EndPaint(hWnd,&ps);

	CHECK_REGISTRATION_2( 22, CLEAR_COUNTER, 40, ACT_SET_NOMENU_ICONS );
}


/*
 *
 *
 */
HICON DottedIcon( HICON hIcon, int w, int h )
{
	HDC     hDC    = GetDC(GetDesktopWindow());
	HDC     hMemDC = CreateCompatibleDC( hDC );
	HBITMAP hBmp   = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_LOGO1) );
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBmp );

	HBRUSH hBrush    = CreateSolidBrush(GetSysColor(COLOR_MENU));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC,hBrush);
	DrawIconEx( hMemDC, 0,0, hIcon, w,h, 0, hBrush, DI_NORMAL );
	SelectObject(hMemDC, hOldBrush );
	DeleteObject(hBrush);

	hBrush    = CreateSolidBrush(RGB(255,255,0));
	hOldBrush = (HBRUSH)SelectObject(hMemDC,hBrush);
	Rectangle(hMemDC,w-w/3,h-h/3,w,h);
	SelectObject(hMemDC, hOldBrush );
	SelectObject(hMemDC, hOldBitmap );

	HIMAGELIST hImageList = ImageList_Create( w, h, ILC_COLORDDB|ILC_MASK, 0, 1 ); 
	ImageList_Add( hImageList, hBmp, 0 );
	HICON hResultIcon = ImageList_ExtractIcon( hInst,  hImageList, 0 );
	ImageList_Destroy( hImageList );

	DeleteObject(hBrush);
	DeleteObject(hBmp);
	DeleteDC(hMemDC);
	ReleaseDC(GetDesktopWindow(),hDC);

	return hResultIcon;
}


/*
 *
 *
 */
HICON CpuIcon( HICON hIcon, int w, int h, int iCpuUsage )
{
	HDC     hDC    = GetDC(GetDesktopWindow());
	HDC     hMemDC = CreateCompatibleDC( hDC );
	HBITMAP hBmp   = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_LOGO1) );
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBmp );

	HBRUSH hBrush    = CreateSolidBrush(GetSysColor(COLOR_MENU));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC,hBrush);
	DrawIconEx( hMemDC, 0,0, hIcon, w,h, 0, hBrush, DI_NORMAL );
	SelectObject(hMemDC, hOldBrush );
	DeleteObject(hBrush);

	hBrush    = CreateSolidBrush(RGB(255,255,0));
	hOldBrush = (HBRUSH)SelectObject(hMemDC,hBrush);
	Rectangle(hMemDC,w-w/3,h-h/3,w,h);
	SelectObject(hMemDC, hOldBrush );
	SelectObject(hMemDC, hOldBitmap );

	HIMAGELIST hImageList = ImageList_Create( w, h, ILC_COLORDDB|ILC_MASK, 0, 1 ); 
	ImageList_Add( hImageList, hBmp, 0 );
	HICON hResultIcon = ImageList_ExtractIcon( hInst,  hImageList, 0 );
	ImageList_Destroy( hImageList );

	DeleteObject(hBrush);
	DeleteObject(hBmp);
	DeleteDC(hMemDC);
	ReleaseDC(GetDesktopWindow(),hDC);

	return hResultIcon;
}

/*
 *
 *
 */
void DrawTransparent(HBITMAP hBitmap, HDC hDC, int x, int y, COLORREF crColour)
{
	COLORREF crOldBack = SetBkColor(hDC,RGB(255,255,255));
	COLORREF crOldText = SetTextColor(hDC,RGB(0,0,0));

	// Create two memory dcs for the image and the mask
	HDC dcImage = CreateCompatibleDC(hDC);
	HDC dcTrans = CreateCompatibleDC(hDC);

	// Select the image into the appropriate dc
	HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(dcImage,hBitmap);

	// Create the mask bitmap
	BITMAP bm;
	::GetObject(hBitmap, sizeof(BITMAP), &bm);
	HBITMAP bitmapTrans = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	HBITMAP pOldBitmapTrans = (HBITMAP)SelectObject(dcTrans,bitmapTrans);

	// Build mask based on transparent colour
	SetBkColor(dcImage,crColour);
	BitBlt(dcTrans,0, 0, bm.bmWidth, bm.bmHeight, dcImage, 0, 0, SRCCOPY);

	// Do the work - True Mask method - cool if not actual display
	BitBlt(hDC, x, y, bm.bmWidth, bm.bmHeight, dcImage, 0, 0, SRCINVERT);
	BitBlt(hDC, x, y, bm.bmWidth, bm.bmHeight, dcTrans, 0, 0, SRCAND);
	BitBlt(hDC, x, y, bm.bmWidth, bm.bmHeight, dcImage, 0, 0, SRCINVERT);

	// Restore settings
	SelectObject(dcImage,pOldBitmapImage);
	SelectObject(dcTrans,pOldBitmapTrans);
	SetBkColor(hDC,crOldBack);
	SetTextColor(hDC,crOldText);

	DeleteObject(bitmapTrans);
	DeleteObject(dcImage);
	DeleteObject(dcTrans);
}

