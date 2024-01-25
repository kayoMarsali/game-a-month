REM Build script for testbed
@ECHO OFF
SetLocal EnableDelayedExpansion

SET CONFIG=%1

if "%CONFIG%" == "clean" goto clean
if "%CONFIG%" == "release" goto build
if "%CONFIG%" == "debug" goto build
goto notdefined

:build
if not exist "%cd%\bin\" mkdir "%cd%\bin\"
 
REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)


echo "Files:" %cFilenames%

SET assembly=KayoPong
if "%CONFIG%" == "release" goto release
SET compilerFlags=-g
SET defines=-D_DEBUG
goto link
:release
SET compilerFlags=-O2
SET defines=-D_RELEASE

:link
SET includeFlags=-Isrc -I%SDL2%/include -I%SDL2_ttf%/include
SET linkerFlags= -L%SDL2%/lib/x64 -L%SDL2_ttf%\lib\x64 -lSDL2 -lSDL2main -l:SDL2.dll -lSDL2_ttf -l:SDL2_ttf.dll
if not exist "%cd%\bin\%CONFIG%\" mkdir "%cd%\bin\%CONFIG%\"

ECHO "Building %assembly%%..."
gcc %cFilenames% %compilerFlags% -o ./bin/%CONFIG%\%assembly%.exe %defines% %includeFlags% %linkerFlags%

ECHO "Copying DLL(s)..."
copy /y "%SDL2%\lib\x64\SDL2.dll" /b ".\bin\%CONFIG%\"
copy /y "%SDL2_ttf%\lib\x64\SDL2_ttf.dll" /b ".\bin\%CONFIG%\"
xcopy "assets" "bin\%CONFIG%\assets" /h /i /c /k /e /r /y
goto end

:clean
ECHO "Cleaning..."
if exist ".\bin\" rmdir /S /Q ".\bin\"
goto end
:notdefined
ECHO "Config not defined, please add 'release' 'debug' or 'clean' to the command line when running ./build.bat."


:end