rmdir /q /s ..\\Intermediate
rmdir /q /s ..\\Saved\\Autosaves
rmdir /q /s ..\\Saved\\Backup
rmdir /q /s ..\\Saved\\Collections
rmdir /q /s ..\\Saved\\Config
rmdir /q /s ..\\Saved\\Crashes
rmdir /q /s ..\\Saved\\Logs

rmdir /q /s ..\\.vs
rmdir /q /s ..\\Binaries

rmdir /q /s ..\\Plugins\\ExtraCameraWindow\\Binaries
rmdir /q /s ..\\Plugins\\ExtraCameraWindow\\Intermediate
rmdir /q /s ..\\Plugins\\FullbodyIK\\Binaries
rmdir /q /s ..\\Plugins\\FullbodyIK\\Intermediate
rmdir /q /s ..\\Plugins\\LocomotionSystem\\Binaries
rmdir /q /s ..\\Plugins\\LocomotionSystem\\Intermediate
del ..\\Wevet.sln

pause
exit 0