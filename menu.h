#ifndef __MENU_H__
#define __MENU_H__

#include <vector>
#include <shlobj.h>

enum CMenuItemType {
	MIT_HWND,
	MIT_PIDL,
	MIT_DIALUP_NAME
};

typedef std::vector<int> CMenuItemVector;
typedef std::vector<int> CMenuItemTypeVector;

typedef std::vector<int> CHotkeyPosVector;


/*
 *
 *
 */
class CCustomMenu 
{
public:
	CCustomMenu	();
	~CCustomMenu();

	void  Initialize ( HMENU hMenu, int MenuType );
	HMENU GetMenu() { return hMenu; }
	
	int   AddItem ( int item, CMenuItemType type );
	void  Sort();

	void  OnMeasureItem( HWND hwnd, LPMEASUREITEMSTRUCT lpmis );
	void  OnDrawItem( LPDRAWITEMSTRUCT lpdis );
	void  OnExecute( int ID );
	
	LRESULT OnMenuChar( char chPressedChar, HMENU hMenu );
	
private:
	int					m_MenuType;
	HFONT				hFont;
	HMENU				hMenu;
	CMenuItemVector		items;
	CMenuItemTypeVector types;
	CHotkeyPosVector    hkey;

	int					m_iCurrMenuItem;
	HMENU				m_hCurrSubMenu;

	void  FreeVector ();

	void  GetItemInfo( int ID, char* str, HICON* phIcon = 0 );	

	void  SpecGetItemText( HMENU hmenu, int ID, char* str );	
};


extern CCustomMenu CustomMenu;


#endif