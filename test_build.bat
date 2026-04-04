@echo off
setlocal enabledelayedexpansion

cd /d "C:\Users\qveezzx\Documents\Changer"

echo Starting build...
msbuild "src\ext-cs2-skin-changer.sln" /p:Configuration=Release /p:Platform=x64 /m > build_output.txt 2>&1

echo.
echo === BUILD OUTPUT ===
type build_output.txt

echo.
echo === ERRORS ONLY ===
findstr /i "error" build_output.txt

pause
