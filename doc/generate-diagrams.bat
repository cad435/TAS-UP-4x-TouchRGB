@echo off
REM Batch script to convert all Mermaid diagrams to PNG
REM Requires: npm install -g @mermaid-js/mermaid-cli

echo ========================================
echo Mermaid Diagram to PNG Converter
echo ========================================
echo.

REM Check if mmdc is installed
where mmdc >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: mermaid-cli is not installed!
    echo.
    echo Please install it first:
    echo   npm install -g @mermaid-js/mermaid-cli
    echo.
    pause
    exit /b 1
)

echo Found mermaid-cli, starting conversion...
echo.

REM Create output directory if it doesn't exist
if not exist "png" mkdir png

REM Counter for processed files
set count=0

REM Process each .md file (excluding README.md)
for %%f in (*.md) do (
    if not "%%f"=="README.md" (
        echo Converting %%f...
        mmdc -i "%%f" -o "png\%%~nf.png" -b transparent -t dark
        if %errorlevel% equ 0 (
            set /a count+=1
            echo   [OK] Created png\%%~nf.png
        ) else (
            echo   [FAILED] Could not convert %%f
        )
        echo.
    )
)

echo ========================================
echo Conversion complete!
echo Processed %count% diagram(s)
echo Output location: png\
echo ========================================
echo.

pause
