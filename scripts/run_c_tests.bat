@echo off

call :run_test .\build\jctest_c.exe
call :run_test .\build\jctest_c_reporting.exe
call :run_test .\build\jctest_c_failure_allocation.exe
call :run_test .\build\jctest_c_default_main.exe
call :run_test .\build\jctest_c_filtering.exe
call :run_test .\build\jctest_c_color.exe
exit /b 0

:run_test
if not exist "%~1" (
    echo Missing test executable: %~1
    exit /b 1
)
call "%~1"
if errorlevel 1 exit /b %errorlevel%
exit /b 0
