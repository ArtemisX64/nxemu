@echo off
SETLOCAL

set origdir=%cd%
cd /d %~dp0..\..
set base_dir=%cd%
cd /d %origdir%

if "%~4"=="" (
    echo Usage: update_version.cmd [BuildMode] [Platform] [InFile] [OutFile]
    goto :eof
)

if not "%1" == "" set BuildMode=%~1
if not "%~2" == "" set Platform=%~2
if not "%~2" == "" set InFile="%~3"
if not "%~3" == "" set OutFile="%~4"

echo "%base_dir%\Bin\%Platform%\%BuildMode%\update_version.exe" %InFile% %OutFile%
"%base_dir%\Bin\%Platform%\%BuildMode%\update_version.exe" %InFile% %OutFile%
