@rem    This batch file saves DoHide settings from the registry
@rem    to files DoHide1.reg and DoHide2.reg

regedit /e DoHide1.reg HKEY_CURRENT_USER\Software\MrKirienko\DoHide
regedit /e DoHide2.reg HKEY_LOCAL_MACHINE\Software\MrKirienko\DoHide
