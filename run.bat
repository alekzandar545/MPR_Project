@echo off
echo === Starting build ===

set BUILD_DIR=build

REM --- Clean previous build ---
if exist %BUILD_DIR% (
    echo Cleaning old build...
    rmdir /s /q %BUILD_DIR%
)

REM --- Create build directory ---
echo Creating build directory...
mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM --- Configure CMake with MinGW ---
echo Configuring CMake...
cmake .. -G "MinGW Makefiles"
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

REM --- Build everything (client + server) ---
echo Building project...
mingw32-make
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo === Build Successful! ===

echo Binaries are in: %BUILD_DIR%\bin\
echo   -> client.exe
echo   -> server.exe
echo.

pause
