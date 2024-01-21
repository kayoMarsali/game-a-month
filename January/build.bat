REM Build script for testbed
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

echo "Files:" %cFilenames%

SET assembly=KayoPong
SET compilerFlags=-g
SET includeFlags=-Isrc -I%SDL2%/include
SET linkerFlags= -L%SDL2%/lib/x64 -lSDL2 -lSDL2main -l:SDL2.dll
SET defines=-D_DEBUG

ECHO "Building %assembly%%..."
gcc %cFilenames% %compilerFlags% -o ./bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%

ECHO "Copying DLL(s)..."
copy /y "%SDL2%\lib\x64\SDL2.dll" /b ".\bin\%CONFIG%\"
