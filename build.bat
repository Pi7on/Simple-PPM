@echo off

set OPT_LEVEL=/O2

pushd bin
cl -Zi %OPT_LEVEL% /nologo ..\src\main.c ..\src\ppm.c
popd