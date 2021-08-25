@echo off

echo %0:starting... 
pushd bin
call main.exe %*
popd