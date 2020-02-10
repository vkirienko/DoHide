#include "StdAfx.h"
#include <shlobj.h>
#include <objbase.h>

/*
 *
 *
 */
IMalloc* GetMalloc()
{
	static IMalloc* g_pMalloc = 0;
	
	if ( g_pMalloc==0 )
		::SHGetMalloc(&g_pMalloc);

	return g_pMalloc;
}


/*
 *
 *
 */
IShellFolder* GetDesktopFolder()
{
	static IShellFolder* g_pDesktopFolder = 0;
	
	if ( g_pDesktopFolder==0 ) 
		::SHGetDesktopFolder(&g_pDesktopFolder);

	return g_pDesktopFolder;
}



/*
 *
 *
 */
HRESULT StrRetToStr(STRRET StrRet, LPTSTR* str, LPITEMIDLIST pidl)
{
	HRESULT hr = S_OK;
	int cch;
	LPSTR strOffset;

	*str = NULL;  // Assume failure

	switch (StrRet.uType)
    {
		case STRRET_WSTR: 
			cch = WideCharToMultiByte(CP_ACP, 0, StrRet.pOleStr, -1, NULL, 0, NULL, NULL); 
			*str = (LPTSTR)GetMalloc()->Alloc(cch * sizeof(TCHAR)); 

			if (*str != NULL)
				WideCharToMultiByte(CP_ACP, 0, StrRet.pOleStr, -1, *str, cch, NULL, NULL); 
			else
				hr = E_FAIL;
			break;

		case STRRET_OFFSET: 
			strOffset = (((char *) pidl) + StrRet.uOffset);

			cch = lstrlen(strOffset) + 1; // NULL terminator
			*str = (LPTSTR)GetMalloc()->Alloc(cch * sizeof(TCHAR));

			if (*str != NULL)
				strcpy(*str, strOffset);
			else
				hr = E_FAIL;
			break;

		case STRRET_CSTR: 
			cch = lstrlen(StrRet.cStr) + 1; // NULL terminator
			*str = (LPTSTR)GetMalloc()->Alloc(cch * sizeof(TCHAR));

			if (*str != NULL)
				strcpy(*str, StrRet.cStr);
			else
				hr = E_FAIL;

			break;
	} 

	return hr;
}


/*
 *
 *
 */
int GetItemIDCount(LPCITEMIDLIST pidl)
{
	if ( !pidl ) 
		return 0;
	
	int nCount = 0;
	BYTE* pCur = (BYTE*)pidl;
	
	while ( ((LPCITEMIDLIST)pCur)->mkid.cb ) 
	{
		nCount ++;
		pCur += ((LPCITEMIDLIST)pCur)->mkid.cb;
	}
	
	return nCount;
}


/*
 * get size of PIDL - returns PIDL size (even if it's composite
 * PIDL - i.e. composed from more than one PIDLs)
 *
 */
int GetItemIDSize(LPCITEMIDLIST pidl) 
{
	if ( !pidl ) 
		return 0;
	
	int nSize = 0;
	while( pidl->mkid.cb ) 
	{
		nSize += pidl->mkid.cb;
		pidl = (LPCITEMIDLIST)(((LPBYTE)pidl) + pidl->mkid.cb);
	}
	
	return nSize;
}

/*
 *
 *
 */
LPITEMIDLIST CopyItemID(LPCITEMIDLIST pidl, int cb=-1) 
{ 
	if ( GetMalloc()==0 ) 
		return NULL;

	//	Get the size of the specified item identifier. 
	if ( cb == -1 )
		cb = GetItemIDSize(pidl); 

	//	Allocate a new item identifier list. 
	LPITEMIDLIST pidlNew = (LPITEMIDLIST)GetMalloc()->Alloc(cb+sizeof(USHORT)); 
	if (pidlNew == NULL) 
		return NULL; 

	//	Copy the specified item identifier. 
	CopyMemory(pidlNew, pidl, cb); 

	//	Append a terminating zero. 
	*((USHORT *)(((LPBYTE) pidlNew) + cb)) = 0; 

	return pidlNew; 
}


/*
 *
 *
 */
LPBYTE GetItemIDPos(LPCITEMIDLIST pidl, int nPos)
{
	if ( !pidl ) 
		return 0;
	
	int nCount = 0;
	BYTE* pCur = (BYTE*)pidl;
	
	while( ((LPCITEMIDLIST)pCur)->mkid.cb ) 
	{
		if( nCount == nPos ) 
			return pCur;
	
		nCount ++;
		pCur += ((LPCITEMIDLIST)pCur)->mkid.cb;// + sizeof(pidl->mkid.cb);
	}
	
	if( nCount == nPos ) 
		return pCur;

	return NULL;
}


/*
 *
 *
 */
HRESULT _SHBindToParent(LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast)
{
	HRESULT hr;
	
	if ( !pidl || !ppv )
		return E_POINTER;
	
	int nCount = ::GetItemIDCount(pidl);
	if ( nCount == 0 ) 
	{
		//	this is desktop pidl or invalid PIDL
		return E_POINTER;
	}
	
	if ( nCount == 1 ) 
	{
		//	this is desktop item
		if ( SUCCEEDED(hr = GetDesktopFolder()->QueryInterface(riid, ppv)) )
		{
			if ( ppidlLast ) 
				*ppidlLast = ::CopyItemID(pidl);
		}
	
		return hr;
	} 

	LPBYTE pRel = ::GetItemIDPos(pidl, nCount-1);
	LPCITEMIDLIST pidlPar = ::CopyItemID(pidl, pRel-(LPBYTE)pidl);
	IShellFolder* pFolder;
	
	if ( FAILED(hr = GetDesktopFolder()->BindToObject(pidlPar, NULL, IID_IShellFolder, (void**)&pFolder)) )
	{
		return hr;
	}
	
	if ( SUCCEEDED(hr = pFolder->QueryInterface(riid, ppv)) )
	{
		if ( ppidlLast ) 
			*ppidlLast = ::CopyItemID((LPCITEMIDLIST)pRel);
	}
	
	return hr;
}



/*
 *
 *
 */
BOOL GetShellItemName( LPCITEMIDLIST pidl,  DWORD dwFlags, char* sDisplayName)
{
	SHFILEINFO sfi;
	IShellFolder* pFolder;
	LPCITEMIDLIST pidlRel;
	
	if ( SUCCEEDED(_SHBindToParent(pidl, IID_IShellFolder,(void**)&pFolder, &pidlRel)) )
	{
		STRRET str;

		if ( SUCCEEDED(pFolder->GetDisplayNameOf(pidlRel, dwFlags, &str)) )
		{
		    LPTSTR lpszName = NULL;
			StrRetToStr(str, &lpszName, (LPITEMIDLIST)pidlRel);
			lstrcpy(sDisplayName,lpszName);
		}
	} 
	else
	if ( ::SHGetFileInfo((LPCTSTR)pidl, NULL, &sfi, sizeof(sfi), SHGFI_PIDL|SHGFI_DISPLAYNAME) )
	{
		lstrcpy( sDisplayName, sfi.szDisplayName );
	} 
	else
	{
		return FALSE;
	}

	return TRUE;
}


/* 
 *
 *
 */
LPITEMIDLIST MergeItemID(LPCITEMIDLIST pidl,...) 
{
	va_list marker;
	int nSize = GetItemIDSize(pidl) + sizeof(pidl->mkid.cb);
	LPITEMIDLIST p;

	//	count size of pidl
	va_start(marker,pidl);
	while ( p = va_arg(marker, LPITEMIDLIST) )
		nSize += GetItemIDSize(p);
	va_end(marker);

	//	allocate and merge pidls
	LPITEMIDLIST pidlNew = (LPITEMIDLIST)GetMalloc()->Alloc(nSize); 
	if ( pidlNew == NULL ) 
		return NULL; 

	va_start(marker,pidl);
    CopyMemory(((LPBYTE)pidlNew), pidl, nSize = GetItemIDSize(pidl)); 
	while( p = va_arg(marker, LPITEMIDLIST) ) 
	{
		CopyMemory(((LPBYTE)pidlNew) + nSize, p, GetItemIDSize(p)); 
		nSize += p->mkid.cb;
	}
	va_end(marker);
	
	*((USHORT *)(((LPBYTE) pidlNew) + nSize)) = 0; 

	return pidlNew; 
}
