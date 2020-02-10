#ifndef __BITMAP_H__
#define __BITMAP_H__

extern HICON DottedIcon( HICON hIcon, int w, int h );
extern HICON CpuIcon( HICON hIcon, int w, int h, int iCpuUsage );
extern void  DrawTransparent(HBITMAP hBitmap, HDC hDC, int x, int y, COLORREF crColour);


/*
 *
 *
 */
class TBitmap
{
public:

	TBitmap( UINT id, bool isBitmap=true ) 
	{ 
		m_bBitmap = isBitmap;
		SetId(id);
	}

	void Init( HWND hWnd );
	void Paint( HWND hWnd );
	void SetId(int id) { m_ID=id; }

	int  Height(HBITMAP hBmp);
	int  Width(HBITMAP hBmp);

	bool    m_bBitmap;
	UINT    m_ID;
	WNDPROC wpOrigBitmapProc;
};

#endif