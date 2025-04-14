@echo off
setlocal

set /A loop=%~1
if %loop%==0 set loop=1000
set /A comp=%~2
if %comp%==0 set comp=1

if %comp%==1 (
    echo Compiling...
    g++ gen.cpp -o gen
    g++ solution.cpp -o solution
    g++ brute.cpp -o brute
)

for /L %%i in (1,1,%loop%) do (
    echo Test %%i
    gen > in.txt
    solution < in.txt > out1.txt
    brute < in.txt > out2.txt
    fc out1.txt out2.txt > nul
    if errorlevel 1 (
        echo Mismatch found!
        echo === Input ===
        type in.txt
        echo.
        echo === Output ===
        type out1.txt
        echo.
        echo === Expected ===
        type out2.txt
        goto :end
    )
)

echo All tests passed!
:end
del in.txt out1.txt out2.txt > nul 2>&1




rem use the following command to run the script
rem ./stress.bat 1000 1