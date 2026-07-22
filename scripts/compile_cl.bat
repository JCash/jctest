@echo off

call python3 --version 2>NUL
if '%errorlevel%'=='1' goto errorNoPython
set TIMEIT=python3 %~dp0\timeit.py
goto hasPython
:errorNoPython
echo "No python found!"
exit /b 1

set TIMEIT
:hasPython

if not exist build mkdir build

set FLAGS=/Od /Zi /D_CRT_SECURE_NO_WARNINGS /nologo /D_HAS_EXCEPTIONS=0 /EHsc /W4 /wd4611 /Isrc
if not "%USE_STATICANALYZE%"=="" (
    set FLAGS=%FLAGS% /analyze /WX
    echo Using STATIC ANALYZER (msvc)
)

call %TIMEIT% cl.exe %FLAGS% test\test_params.cpp test\main.cpp /link /out:.\build\jctest_cpp_params.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_typed_test.cpp test\main.cpp /link /out:.\build\jctest_cpp_typed_test.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_expect.cpp test\main.cpp /link /out:.\build\jctest_cpp_expect.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_death.cpp test\main.cpp /link /out:.\build\jctest_cpp_death.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_empty.cpp test\main.cpp /link /out:.\build\jctest_cpp_empty.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_array.cpp test\main.cpp /link /out:.\build\jctest_cpp_array.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_buffered_string.cpp /link /out:.\build\jctest_cpp_buffered_string.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_reporting.cpp /link /out:.\build\jctest_cpp_reporting.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_color_off.cpp /link /out:.\build\jctest_cpp_color_off.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_color_on.cpp /link /out:.\build\jctest_cpp_color_on.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_failure_recap.cpp /link /out:.\build\jctest_cpp_failure_recap.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_failure_allocation.cpp /link /out:.\build\jctest_cpp_failure_allocation.exe
if errorlevel 1 exit /b %errorlevel%
call %TIMEIT% cl.exe %FLAGS% test\test_filtering.cpp /link /out:.\build\jctest_cpp_filtering.exe
if errorlevel 1 exit /b %errorlevel%

del *.obj

exit /b 0
