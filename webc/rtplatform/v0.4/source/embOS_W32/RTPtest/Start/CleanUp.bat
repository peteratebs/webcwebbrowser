@echo off

if "%os%" == "Windows_NT" goto Windows_NT
goto end

:Windows_NT

rd exe /s /q

if exist *.opt del *.opt /s /q
if exist *.plg del *.plg /s /q
if exist *.aps del *.aps /s /q
if exist *.ncb del *.ncb /s /q
if exist Output rd output /s /q
if exist Debug  rd Debug  /s /q

:end

rem end