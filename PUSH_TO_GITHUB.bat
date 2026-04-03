@echo off
REM CS2 Skin Changer - Web Server Launcher
REM This script starts the Node.js web interface

:: THIS IS THE FIX: Set the working directory to the script's folder
cd /d "%~dp0"

setlocal enabledelayedexpansion

echo.
echo ╔════════════════════════════════════════════╗
echo ║    CS2 Skin Changer - Web Server Launcher    ║
╚════════════════════════════════════════════╝
echo.

REM Check if Node.js is installed
node --version >nul 2>&1
if errorlevel 1 (
    echo ❌ ERROR: Node.js is not installed or not in PATH
    echo.
    echo Please install Node.js from: https://nodejs.org/
    echo Then run this script again.
    echo.
    pause
    exit /b 1
)

echo ✓ Node.js detected
node --version
echo.

REM Check if dependencies are installed
if not exist "node_modules" (
    echo 📦 Installing dependencies...
    echo.
    call npm install express ws cors dotenv
    if errorlevel 1 (
        echo ❌ Failed to install dependencies
        pause
        exit /b 1
    )
    echo ✓ Dependencies installed
    echo.
)

REM Start the server
echo 🚀 Starting CS2 Skin Changer Web Server...
echo.
echo 🌐 Server will be available at: http://localhost:3000
echo.
echo Press Ctrl+C to stop the server
echo.

node web-server.js

pause