@echo off

:: build       --> build only
:: build run   --> run only
:: build clean --> clean

:: Project related variables
set BIN_PATH=bin\windows
set PROG_NAME="main.exe"
set SRC_FILES="..\..\src\*.c"

:: Compiler stuff
set CC="cl"
set CC_FLAGS=-Zi /W4 /O2 /nologo /Fe%PROG_NAME%

:: parse argumets
IF "%1%"=="env" goto CASE_INIT_ENV
IF "%1%"=="" goto CASE_BUILD_ONLY
IF "%1%"=="run" goto CASE_RUN
IF "%1%"=="clean" goto CASE_CLEAN

:CASE_INIT_ENV
    :: calls vcvarsall.bat to initilize msvc env variables
    start /b /wait initcl

:CASE_BUILD_ONLY
    pushd %BIN_PATH%
    %CC%  %CC_FLAGS%  %SRC_FILES%
    echo.
    popd
    goto END

:CASE_RUN
    pushd %BIN_PATH%
    call %PROG_NAME%
    popd
    goto END

:CASE_CLEAN
    pushd %BIN_PATH%
    echo %0:cleaning...
    del /Q *
    popd
    goto END

:END