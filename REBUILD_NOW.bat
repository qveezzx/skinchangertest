@echo off
cd /d "C:\Users\qveezzx\Documents\Changer"
echo Rebuilding...
msbuild "src\ext-cs2-skin-changer.sln" /m /p:Configuration=Release /p:Platform=x64 /nologo /nr:false
if %errorlevel% equ 0 (
    echo.
    echo ✅ BUILD SUCCESSFUL!
    echo.
) else (
    echo.
    echo ❌ BUILD FAILED
    echo.
)
pause
