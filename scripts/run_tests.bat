@echo off

call :run_test .\build\jctest_cpp_params.exe
call :run_test .\build\jctest_cpp_typed_test.exe
call :run_test .\build\jctest_cpp_expect.exe
call :run_test .\build\jctest_cpp_death.exe
call :run_test .\build\jctest_cpp_empty.exe
call :run_test .\build\jctest_cpp_array.exe
call :run_test .\build\jctest_cpp_buffered_string.exe
call :run_test .\build\jctest_cpp_reporting.exe
call :run_test .\build\jctest_cpp_color_off.exe
call :run_test .\build\jctest_cpp_color_on.exe
call :run_test .\build\jctest_cpp_failure_recap.exe
call :run_test .\build\jctest_cpp_failure_allocation.exe
call :run_test .\build\jctest_cpp_filtering.exe
exit /b 0

:run_test
if not exist "%~1" (
    echo Missing test executable: %~1
    exit /b 1
)
call "%~1"
if errorlevel 1 exit /b %errorlevel%
exit /b 0
