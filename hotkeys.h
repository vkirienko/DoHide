#ifndef __HOTKEYS_H__
#define __HOTKEYS_H__

#define COUNT_HOTKEY 50


/*
 *
 *
 */
class THotKeys 
{
public:
	THotKeys();
	~THotKeys();

	void LoadFromRegistry();
	void StoreToRegistry();

	void RegisterAllHotKey ();
	void UnregisterAllHotKey ();

	UINT GetKey( int i ) { return m_uHotKey[i]; }
	void SetKey( int i, int key ) { m_uHotKey[i]=key; }

	UINT IsAddToMenu( int i ) { return m_uAddToMenu[i]; }
	void SetAddToMenu( int i, UINT add ) { m_uAddToMenu[i]=add; }

#ifdef NEW_DEFENCE
	void ClearHotKeys () { ZeroMemory(&m_uHotKey[0],sizeof(m_uHotKey)); }
#endif

	int Translate( int i );

private:
	UINT GetMod ( DWORD hk );

	UINT m_uHotKey[COUNT_HOTKEY];
	UINT m_uAddToMenu[COUNT_HOTKEY];
};


extern THotKeys HotKeys;


#endif