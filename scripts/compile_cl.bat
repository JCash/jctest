rem echo off

call python3 --version 2>NUL
if '%errorlevel%'=='1' goto errorNoPython
set TIMEIT=python %~dp0\timeit.py
goto hasPython
:errorNoPython
echo "No python found!"
goto end

set TIMEIT
:hasPython

mkdir build

set FLAGS=/Od /Zi /D_CRT_SECURE_NO_WARNINGS /nologo /D_HAS_EXCEPTIONS=0 /EHsc /W4 /wd4611 /Isrc

call %TIMEIT% cl.exe %FLAGS% test\test_params.cpp test\main.cpp /link /out:.\build\test_params.exe
call %TIMEIT% cl.exe %FLAGS% test\test_typed_test.cpp test\main.cpp /link /out:.\build\test_typed_test.exe
call %TIMEIT% cl.exe %FLAGS% test\test_expect.cpp test\main.cpp /link /out:.\build\test_expect.exe
call %TIMEIT% cl.exe %FLAGS% test\test_death.cpp test\main.cpp /link /out:.\build\test_death.exe
call %TIMEIT% cl.exe %FLAGS% test\test_empty.cpp test\main.cpp /link /out:.\build\test_empty.exe
call %TIMEIT% cl.exe %FLAGS% test\test_array.cpp test\main.cpp /link /out:.\build\test_array.exe
call %TIMEIT% cl.exe %FLAGS% test\test_color_off.cpp /link /out:.\build\test_color_off.exe
call %TIMEIT% cl.exe %FLAGS% test\test_color_on.cpp /link /out:.\build\test_color_on.exe

del *.obj

:end
