@echo off
cd /d "C:\Users\qveezzx\Documents\Changer"
echo Building CS2 Skin Changer...
msbuild "src\ext-cs2-skin-changer.sln" /m /p:Configuration=Release /p:Platform=x64 /fl /flp:verbosity=detailed;logfile=build.log
if %errorlevel% equ 0 (
    echo.
    echo BUILD SUCCESSFUL!
    echo.
) else (
    echo.
    echo BUILD FAILED - Check build.log for details
    echo.
)
pause
