@echo off

if not exist build mkdir build
if "%CSTDVERSION%"=="" set CSTDVERSION=c11

echo COMPILING C TESTS
echo Using /std:%CSTDVERSION%

set C_TEST_FLAGS=/Od /Zi /std:%CSTDVERSION% /TC /D_CRT_SECURE_NO_WARNINGS /nologo /W4 /Isrc %CFLAGS%
call cl.exe %C_TEST_FLAGS% test\c\main.c test\c\test_assertions.c test\c\test_fixture.c /link /out:.\build\jctest_c.exe
if errorlevel 1 exit /b %errorlevel%
call cl.exe %C_TEST_FLAGS% test\c\test_reporting.c /link /out:.\build\jctest_c_reporting.exe
if errorlevel 1 exit /b %errorlevel%
call cl.exe %C_TEST_FLAGS% test\c\test_failure_allocation.c /link /out:.\build\jctest_c_failure_allocation.exe
if errorlevel 1 exit /b %errorlevel%
call cl.exe %C_TEST_FLAGS% test\c\test_default_main.c /link /out:.\build\jctest_c_default_main.exe
if errorlevel 1 exit /b %errorlevel%
call cl.exe %C_TEST_FLAGS% test\c\test_filtering.c /link /out:.\build\jctest_c_filtering.exe
if errorlevel 1 exit /b %errorlevel%
call cl.exe %C_TEST_FLAGS% test\c\test_color.c /link /out:.\build\jctest_c_color.exe
if errorlevel 1 exit /b %errorlevel%

cl.exe %C_TEST_FLAGS% /c test\c\compile_fail_ordered_pointer.c /Fo.\build\jctest_c_compile_fail.obj >NUL 2>NUL
if not errorlevel 1 (
    echo Expected ordered pointer assertion compilation to fail
    exit /b 1
)
cl.exe %C_TEST_FLAGS% /c test\c\compile_fail_struct.c /Fo.\build\jctest_c_compile_fail.obj >NUL 2>NUL
if not errorlevel 1 (
    echo Expected struct assertion compilation to fail
    exit /b 1
)

exit /b 0
