@echo off
chcp 65001 >nul 2>&1
setlocal enabledelayedexpansion

echo ============================================
echo   Visual Novel Build Script
echo   Copy-to-English-Path Method
echo ============================================
echo.

set "PROJECT_DIR=e:\trea\视觉系小说\VisualNovel"
set "UE_ENGINE=E:\Epic Games\UE_5.6"
set "BUILD_DIR=E:\VNBuild"
set "BUILD_PROJECT=%BUILD_DIR%\VisualNovel"

echo [1/7] Checking UE Engine...
if not exist "%UE_ENGINE%\Engine\Build\BatchFiles\Build.bat" (
    echo ERROR: UE Engine not found at %UE_ENGINE%
    echo Please update UE_ENGINE variable in this script.
    pause
    exit /b 1
)
echo UE Engine found: %UE_ENGINE%

echo.
echo [2/7] Creating build directory...
if not exist "%BUILD_PROJECT%" mkdir "%BUILD_PROJECT%"
echo Build directory: %BUILD_PROJECT%

echo.
echo [3/7] Copying project files to English path...
echo   From: %PROJECT_DIR%
echo   To:   %BUILD_PROJECT%

copy /y "%PROJECT_DIR%\VisualNovel.uproject" "%BUILD_PROJECT%\VisualNovel.uproject" >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to copy .uproject file
    pause
    exit /b 1
)
echo   .uproject copied

robocopy "%PROJECT_DIR%\Source" "%BUILD_PROJECT%\Source" /E /NFL /NDL /NJH /NJS /NC /NS /NP >nul 2>&1
if %ERRORLEVEL% GTR 7 (
    echo ERROR: Failed to copy Source directory (robocopy error %ERRORLEVEL%)
    pause
    exit /b 1
)
echo   Source/ copied

robocopy "%PROJECT_DIR%\Config" "%BUILD_PROJECT%\Config" /E /NFL /NDL /NJH /NJS /NC /NS /NP >nul 2>&1
if %ERRORLEVEL% GTR 7 (
    echo ERROR: Failed to copy Config directory (robocopy error %ERRORLEVEL%)
    pause
    exit /b 1
)
echo   Config/ copied

if exist "%PROJECT_DIR%\Plugins" (
    robocopy "%PROJECT_DIR%\Plugins" "%BUILD_PROJECT%\Plugins" /E /NFL /NDL /NJH /NJS /NC /NS /NP >nul 2>&1
    echo   Plugins/ copied
)

echo All project files synced.

echo.
echo [4/7] Cleaning intermediate files in build directory...
if exist "%BUILD_PROJECT%\Intermediate" (
    rmdir /s /q "%BUILD_PROJECT%\Intermediate" 2>nul
    echo Intermediate cleaned.
)
if exist "%BUILD_PROJECT%\Binaries" (
    rmdir /s /q "%BUILD_PROJECT%\Binaries" 2>nul
    echo Binaries cleaned.
)
echo Clean build ready.

echo.
echo [5/7] Building project from English path...
echo.
echo   Command: Build.bat VisualNovelEditor Win64 Development "%BUILD_PROJECT%\VisualNovel.uproject" -WaitMutex
echo.

call "%UE_ENGINE%\Engine\Build\BatchFiles\Build.bat" VisualNovelEditor Win64 Development "%BUILD_PROJECT%\VisualNovel.uproject" -WaitMutex

set BUILD_RESULT=%ERRORLEVEL%

echo.
echo [6/7] Copying binaries back to original project...
if %BUILD_RESULT% EQU 0 (
    if exist "%BUILD_PROJECT%\Binaries\Win64\UnrealEditor-VisualNovel.dll" (
        if not exist "%PROJECT_DIR%\Binaries\Win64" mkdir "%PROJECT_DIR%\Binaries\Win64"

        copy /y "%BUILD_PROJECT%\Binaries\Win64\UnrealEditor-VisualNovel.dll" "%PROJECT_DIR%\Binaries\Win64\" >nul 2>&1
        copy /y "%BUILD_PROJECT%\Binaries\Win64\UnrealEditor.modules" "%PROJECT_DIR%\Binaries\Win64\" >nul 2>&1
        copy /y "%BUILD_PROJECT%\Binaries\Win64\VisualNovelEditor.target" "%PROJECT_DIR%\Binaries\Win64\" >nul 2>&1

        echo Binaries copied successfully.
        echo   DLL: %PROJECT_DIR%\Binaries\Win64\UnrealEditor-VisualNovel.dll
    ) else (
        echo WARNING: DLL not found at expected location!
        echo   Expected: %BUILD_PROJECT%\Binaries\Win64\UnrealEditor-VisualNovel.dll
        echo   Searching for DLL...
        dir /s /b "%BUILD_PROJECT%\Binaries\*.dll" 2>nul
    )
) else (
    echo Build failed, skipping binary copy.
)

echo.
echo [7/7] Build result...
if %BUILD_RESULT% EQU 0 (
    echo.
    echo ============================================
    echo   BUILD SUCCEEDED!
    echo ============================================
    echo.
    echo IMPORTANT NEXT STEPS:
    echo   1. CLOSE UE Editor completely (important!)
    echo   2. Delete the Intermediate folder in your project:
    echo      rmdir /s /q "%PROJECT_DIR%\Intermediate"
    echo   3. Reopen the project in UE Editor
    echo   4. Click Play to test the main menu
    echo.
    echo The build copy at %BUILD_PROJECT% is kept for
    echo faster rebuilds next time.
    echo.
) else (
    echo.
    echo ============================================
    echo   BUILD FAILED with error code %BUILD_RESULT%
    echo ============================================
    echo.
    echo Check the error messages above for details.
    echo.
)

pause
