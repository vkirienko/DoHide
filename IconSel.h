#ifndef __ICONSEL_H__
#define __ICONSEL_H__

extern void InitIcon( HWND hWnd );
extern void SetupIcon( HWND hWnd, char* IconFile, int IconId );
extern void UpdateIcon( HWND hSlider, HWND hIconWnd, HWND hIconFileWnd );

#endif