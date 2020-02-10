#ifndef __REGISTER_H__
#define __REGISTER_H__


#include "defend.h" 
#include "hotkeys.h" 
#include "rsa.h" 

/*
 *	Destructive actions
 *
 */
/* 0 */ #define ACT_EMPTY						{ }
/* 1 */ #define ACT_BEEP						{ Beep(1000,100); }
/* 2 */ #define ACT_SHOW_WELCOME_DIALOG			{ PostMessage( hMainDlg, WM_UNREGISTRED, 0, 0 ); }
/* 3 */ #define ACT_REMOVE_PROGRAM_ICONS		{ ProgramList.RemoveAllIcon(); }
/* 3 */ #define ACT_SET_NOMENU_ICONS			{ uKey[10]=uKey[10]=uKey[14]=uKey[16]=1; }
/* 4 */ #define ACT_REMOVE_HIDEBYRUN_LIST		{ HideTitleList.Delete(0); }
/* 5 */ #define ACT_CLEAR_HOTKEY				{ HotKeys.ClearHotKeys(); }
/* 6 */ #define ACT_SEND_EXIT_MESSAGE			{ PostMessage( hMainDlg, WM_COMMAND, IDCANCEL, 1 );  }
/* 7 */ #define ACT_KILL_DOHIDE					{ KillProcess( hMainDlg );  }



#define CLEAR_COUNTER		{ cnt = 0; }
#define IGNORE_COUNTER		{ cnt = cnt; }


#ifdef NEW_DEFENCE
#define CHECK_REGISTRATION( COUNTER, CNT_ACTION, DAYS, ACTION ) \
	{ \
		static int cnt = 0; \
		cnt++; \
		if ( cnt==(COUNTER) ) \
		{ \
			CNT_ACTION; \
			if ( Register->m_DayCounter>(DAYS) && !IS_REGISTRED() ) \
			{ \
				ACTION; \
			}\
		} \
	}

#define CHECK_REGISTRATION_2( COUNTER, CNT_ACTION, DAYS, ACTION ) \
	{ \
		if ( IsTimeToCheckRegistration() ) \
		{ \
			static __int64 cnt = 0; \
			cnt++; \
			if ( cnt==(COUNTER) ) \
			{ \
				if ( Register->m_DayCounter>(DAYS) && !IS_REGISTRED() ) \
				{ \
					ACTION; \
				} \
				CNT_ACTION; \
			} \
		} \
	}
#else
#define CHECK_REGISTRATION( COUNTER, CNT_ACTION, DAYS, ACTION )
#define CHECK_REGISTRATION_2( COUNTER, CNT_ACTION, DAYS, ACTION )
#endif


#ifdef _DEBUG
extern bool IS_REGISTRED();
#else
#define IS_REGISTRED() (Register->IsValidRegistration() && (Register->get_RegCode()==(*Register->m_pGetRegistrationNameCRC)() || Register->get_RegCode()==612686236) )
#endif



typedef unsigned long (*__GetRegistrationNameCRC)();


/*
 *
 *
 */
class TRegistration
{
public:
	TRegistration( );
	~TRegistration( );

	void  StoreToRegistry( );
	void  LoadFromRegistry( );
	bool  IsValidRegistration();
	
	unsigned long getcrc( char* );
	unsigned long get_RegCode( );
	unsigned long get_RegNameCRC( );

	char		m_szUserName[80];
	char		m_szRegCode[15];

	// Dummy
	FILETIME	m_InstallTime;
	// Dummy

	unsigned long crcTable[256];

	BYTE		m_DayCounter;
	BYTE		m_DayOfRun;
	
	__GetRegistrationNameCRC m_pGetRegistrationNameCRC;

	void		UpdateDayCounter();
};


/*
 *
 *
 */
class CSecurity
{
public:
	CSecurity() { ZeroMemory(this,sizeof(CSecurity)); }

	BYTE* computeHash(char* strIn, BYTE hash[16]);
	bool  ReadPassword();

	bool m_bPasswordChanged;
	BYTE m_hash[16];
};


extern TRegistration* Register;
extern CSecurity*	  Security;



#endif