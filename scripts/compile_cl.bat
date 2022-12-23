echo off

echo "Using " %VCINSTALLDIR%

if NOT DEFINED VCINSTALLDIR (
    echo "No compatible visual studio found! run vcvarsall.bat first!"
    exit 1
)

python3 --version 2>NUL
if errorlevel 1 goto errorNoPython
set TIMEIT=python %~dp0/timeit.py
goto hasPython
:errorNoPython
set TIMEIT
:hasPython

mkdir build

set FLAGS=/O2 /D_CRT_SECURE_NO_WARNINGS /nologo /D_HAS_EXCEPTIONS=0 /EHsc /W4 /wd4611 /Isrc

%TIMEIT% cl.exe %FLAGS%% test/test_params.cpp test/main.cpp /link /out:.\build\test_params.exe
%TIMEIT% cl.exe %FLAGS%% test/test_typed_test.cpp test/main.cpp /link /out:.\build\test_typed_test.exe
%TIMEIT% cl.exe %FLAGS%% test/test_expect.cpp test/main.cpp /link /out:.\build\test_expect.exe
%TIMEIT% cl.exe %FLAGS%% test/test_death.cpp test/main.cpp /link /out:.\build\test_death.exe
%TIMEIT% cl.exe %FLAGS%% test/test_empty.cpp test/main.cpp /link /out:.\build\test_empty.exe
%TIMEIT% cl.exe %FLAGS%% test/test_array.cpp test/main.cpp /link /out:.\build\test_array.exe
%TIMEIT% cl.exe %FLAGS%% test/test_doctest.cpp /link /out:.\build\test_doctest.exe
%TIMEIT% cl.exe %FLAGS%% test/test_color_off.cpp /link /out:.\build\test_color_off.exe
%TIMEIT% cl.exe %FLAGS%% test/test_color_on.cpp /link /out:.\build\test_color_on.exe

del *.obj
