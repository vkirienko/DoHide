#include "StdAfx.h"

#include "DoHide.h"
#include "DDE.h"
#include "resource.h"

TDDE DDE;


/*
 * This is the main DDEML callback proc.  It handles all interaction with
 * DDEML that is DDEML originated.
 */
HDDEDATA CALLBACK DdeCallback( WORD wType, WORD wFmt, HCONV hConv, HSZ hszTopic, HSZ hszItem, HDDEDATA hData, DWORD lData1, DWORD lData2)
{
    switch (wType) 
	{
        /*
         * Only allow connections to us.  We can always return TRUE because
         * the CBF_FILTERINITS bit given to DdeInitialize() told DDEML to
         * never bother us with connections to any service names other than
         * what we have registered.
         *
         * Note that we do not handle the XTYP_WILD_CONNECT transaction.
         * This means that no wild-card initiates to us will work.
         */
	    case XTYP_CONNECT:
		    return((HDDEDATA)TRUE);

        /*
         * Only allow links to our Item in our format.
         */
		case XTYP_ADVSTART:
	        return (HDDEDATA)((UINT)wFmt == CF_TEXT && hszItem == DDE.hszAppName);

		/*
		 * Data is comming in.  We don't bother with XTYP_POKE transactions,
		 * but if we did, they would go here.  Since we only allow links
		 * on our item and our format, we need not check these here.
		 */
	    case XTYP_POKE: 
		case XTYP_ADVDATA:
		{
			char s[255];
			if (DdeGetData(hData, (PBYTE)s, sizeof(s), 0)) 
			{
				ProcessDDEAndCmdLine(s);
			}
			/*
			 * This transaction requires a flag return value.  We could also
			 * stick other status bits here if needed but its not recommended.
			 */
			return((HDDEDATA)DDE_FACK);
		}
    }
    return 0;
}


/*
 *
 *
 */
TDDE::TDDE ()
{
	hszAppName = 0;
	hDDEInst = 0;
}


/*
 *
 *
 */
TDDE::~TDDE ()
{
	if ( !hDDEInst )
		return;

    DdeNameService(hDDEInst, 0, 0, DNS_UNREGISTER);
    DdeFreeStringHandle(hDDEInst, hszAppName);
    DdeUninitialize(hDDEInst);
}


/*
 *
 *
 */
void TDDE::Init()
{
    DdeInitialize(&hDDEInst,
            (PFNCALLBACK)MakeProcInstance((FARPROC)DdeCallback, hInst),
            APPCMD_FILTERINITS |
            CBF_SKIP_CONNECT_CONFIRMS |
            CBF_FAIL_SELFCONNECTIONS,
            0);

    /*
     * Initialize all our string handles for lookups later
     */
    hszAppName = DdeCreateStringHandle(hDDEInst, DOHIDE, 0);

    /*
     *  Register our service -
     *  This will cause DDEML to notify DDEML clients about the existance
     *  of a new DDE service.
     */
    DdeNameService(hDDEInst, hszAppName, 0, DNS_REGISTER);
}
