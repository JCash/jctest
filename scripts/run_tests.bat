@echo off

call :run_test .\build\test_params.exe
call :run_test .\build\test_typed_test.exe
call :run_test .\build\test_expect.exe
call :run_test .\build\test_death.exe
call :run_test .\build\test_empty.exe
call :run_test .\build\test_array.exe
call :run_test .\build\test_buffered_string.exe
call :run_test .\build\test_color_off.exe
call :run_test .\build\test_color_on.exe
exit /b 0

:run_test
if not exist "%~1" (
    echo Missing test executable: %~1
    exit /b 1
)
call "%~1"
if errorlevel 1 exit /b %errorlevel%
exit /b 0
