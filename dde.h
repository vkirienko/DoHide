#ifndef __DDE_H__
#define __DDE_H__

class TDDE 
{
public:
	TDDE  ();
	~TDDE ();
	
	void Init();

	HSZ   hszAppName;      // the generic hsz for everything
private:
	DWORD hDDEInst;
};

extern TDDE DDE;

#endif