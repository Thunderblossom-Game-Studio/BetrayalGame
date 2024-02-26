@echo off

set /p port=Port:
set /p password=User:

"C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\Win64\UnrealEditor.exe" "%~dp0\BetrayalGame.uproject" -game -ReX=960 -ResY=540, -log -WINDOWED -AUTH_TYPE="developer" -AUTH_LOGIN="localhost:%port%" -AUTH_PASSWORD="%password%"