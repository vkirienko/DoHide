#ifndef __SHELLAPI_H__
#define __SHELLAPI_H__

extern IMalloc* GetMalloc();
extern IShellFolder* GetDesktopFolder();
extern int		GetItemIDSize(LPCITEMIDLIST pidl);
extern LPITEMIDLIST MergeItemID(LPCITEMIDLIST pidl,...);

extern HRESULT StrRetToStr(STRRET StrRet, LPTSTR* str, LPITEMIDLIST pidl);
extern BOOL GetShellItemName(
				/*in*/  LPCITEMIDLIST pidl,
				/*in*/  DWORD dwFlags,
				/*out*/ char* sDisplayName);


#endif
