#include "StdAfx.h"
#include <shlobj.h>
#include <objbase.h>

#include "ras.h"
#include "defend.h"
#include "register.h"
#include "menu.h"
#include "shell.h"
#include "shellapi.h"
#include "dohide.h"
#include "resource.h"

const char CURR_DIR[]	   = ".";
const char PREV_DIR[]	   = "..";
const char ALL_FILES[]	   = "*.*";	
const char _SLASH_[]       = "\\";
const char LINK[]          = ".lnk";
const char SHORTCUT_NAME[] = "Channel Shortcut";

TShellLink ShellLink;


TShellLink::TShellLink()
{
}

TShellLink::~TShellLink()
{
	GetDesktopFolder()->Release();
}

/*
 *
 *
 */
void TShellLink::ShellFillMenu(HMENU hMenu, int iFillType, LPITEMIDLIST pidl )
{
	IShellFolder* pFolder = GetDesktopFolder(); 
	LPITEMIDLIST pidlFolder; 

	if ( pidl )
	{
		pidlFolder = pidl; 
		if ( GetDesktopFolder()->BindToObject( pidlFolder, 0, IID_IShellFolder, (void**)&pFolder )!=NOERROR )
			return;
	}
	else
	{
		if ( iFillType!=CSIDL_DESKTOP )
		{
			::SHGetSpecialFolderLocation(0, iFillType, &pidlFolder); 
			if ( GetDesktopFolder()->BindToObject( pidlFolder, 0, IID_IShellFolder, (void**)&pFolder )!=NOERROR )
				return;
		}
	}

	IEnumIDList* iel;
	pFolder->EnumObjects(0,SHCONTF_FOLDERS|SHCONTF_NONFOLDERS/*|SHCONTF_INCLUDEHIDDEN*/,&iel);

	if ( !iel )	
		return;

	int cy = GetSystemMetrics(SM_CYSCREEN)/GetSystemMetrics(SM_CYMENUSIZE);
//	cy/=6;

	int count = 2;
	LPITEMIDLIST pItem;
	while ( iel->Next(1,&pItem,0)==NOERROR ) count++;

	pFolder->EnumObjects(0,SHCONTF_FOLDERS|SHCONTF_NONFOLDERS/*|SHCONTF_INCLUDEHIDDEN*/,&iel);
	if ( !iel )	
		return;

	int cnt = 1;
	while ( count/cnt>=cy ) cnt++;
	cy = count/cnt+1;

	int _iMenuItem = 0;
	int iMenuItem  = 0;
	while ( iel->Next(1,&pItem,0)==NOERROR )
	{
		if ( iFillType!=CSIDL_DESKTOP )
		{
			pItem = MergeItemID( pidlFolder, pItem, 0 );
		}

		SHFILEINFO psfi;
		SHGetFileInfo( (LPCTSTR)pItem, NULL, &psfi, sizeof(psfi), SHGFI_PIDL|SHGFI_ATTRIBUTES|SHGFI_TYPENAME);

		char szItemName[128];
		GetShellItemName( pItem, SHGDN_NORMAL, szItemName );

		bool isDirectory = false;
		
		if ( iFillType!=CSIDL_DESKTOP )
			isDirectory = (psfi.dwAttributes & SFGAO_FOLDER) && lstrcmp(psfi.szTypeName, SHORTCUT_NAME)!=0;
		
		HMENU hSubMenu;
		if ( isDirectory )
		{
			hSubMenu = CreatePopupMenu();
			ShellFillMenu(hSubMenu, iFillType, pItem );
		}

		MENUITEMINFO mInfo;
		mInfo.cbSize     = sizeof(MENUITEMINFO);
		mInfo.fMask      = MIIM_ID|MIIM_TYPE|MIIM_DATA|((isDirectory)?MIIM_SUBMENU:0);
		mInfo.fType      = MFT_OWNERDRAW;
		if ( ((iMenuItem+2)%cy)==0 ) mInfo.fType |= MF_MENUBARBREAK;
		mInfo.hSubMenu   = (isDirectory)?hSubMenu:0;
        mInfo.dwItemData = CustomMenu.AddItem( (int)pItem, MIT_PIDL );
		mInfo.wID        = WM_EXECUTE_CUSTOM_MENU + mInfo.dwItemData; 
		InsertMenuItem(hMenu, 2+_iMenuItem, TRUE, &mInfo);
		_iMenuItem++;
		iMenuItem++;
	}

	pFolder->Release();
}


/*
 * 
 *
 */
void TShellLink::ShellFillRasConnectionMenu(HMENU hMenu, int iMenuItem)
{
	ras.EnumEntries( hMenu, iMenuItem );
}



// CreateLink - uses the shell's IShellLink and IPersistFile interfaces 
//   to create and store a shortcut to the specified object. 
// Returns the result of calling the member functions of the interfaces. 
// lpszPathObj - address of a buffer containing the path of the object 
// lpszDesc - address of a buffer containing the description of the shell link 
void CreateStartupLink( LPCSTR lpszPathObj, LPCSTR lpszDesc, LPSTR lpszArguments, LPSTR lpszWorkingDirectory) 
{ 
	char szPathLink[MAX_PATH];
    IShellLink* psl; 
    LPITEMIDLIST pidlPrograms; 

	CoInitialize(NULL);

    if (!SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_STARTUP, &pidlPrograms))) 
		return;
	
    // Get a pointer to the IShellLink interface. 
    if (!SUCCEEDED( CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl))) 
		return; 
	
    IPersistFile* ppf; 
 
    // Set the path to the shortcut target, and add the 
    // description. 
    psl->SetPath(lpszPathObj); 
    psl->SetDescription(lpszDesc); 
	psl->SetArguments(lpszArguments);
	psl->SetWorkingDirectory(lpszWorkingDirectory);

    // Query IShellLink for the IPersistFile interface for saving the 
    // shortcut in persistent storage. 
    if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void**)&ppf)))
	{ 
        WORD wsz[MAX_PATH]; 

        // Ensure that the string is ANSI. 
		SHGetPathFromIDList( pidlPrograms, szPathLink );
		lstrcat(szPathLink,_SLASH_);
		lstrcat(szPathLink,lpszDesc);
		lstrcat(szPathLink,LINK);
        MultiByteToWideChar(CP_ACP, 0, szPathLink, -1, wsz, sizeof(wsz) ); 

        // Save the link by calling IPersistFile::Save. 
        ppf->Save(wsz, TRUE); 
        ppf->Release(); 
    } 
    psl->Release(); 

	CoUninitialize();
	CoFreeUnusedLibraries();
}
 

/*
 *
 *
 */
void DeleteStartupLink( LPCSTR lpszDesc ) 
{
	char szPathLink[MAX_PATH];
    LPITEMIDLIST pidlPrograms; 

	if ( !lpszDesc[0] )
		return ;

    if (!SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_STARTUP, &pidlPrograms))) 
		return ;
	SHGetPathFromIDList( pidlPrograms, szPathLink );

	lstrcat(szPathLink,_SLASH_);
	lstrcat(szPathLink,lpszDesc);
	lstrcat(szPathLink,LINK);

	SetFileAttributes(szPathLink,FILE_ATTRIBUTE_NORMAL); 
	DeleteFile(szPathLink);
}
























