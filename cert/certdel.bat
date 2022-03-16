@echo off

set store=%1
set file=%2

if exist %file% (
   for /F "tokens=3" %%s in ('certutil -dump %file% ^| findstr ^"^^Serial^"') do (
      certutil -user -delstore %store% %%s
   )
)

@echo on
