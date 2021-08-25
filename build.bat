@echo off

pushd bin
cl -Zi ..\src\main.c ..\src\ppm.c
popd