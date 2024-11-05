@ECHO OFF
title Parsing all the syntax trees...
setlocal enabledelayedexpansion

if "%1" NEQ "/n" (
	echo Compiling the compiler...
	g++ ../src/compile.cpp -w -std=c++20 -o ../bin/compile
)

for %%f in (../results/test_code/*.d) do (
    call set in=%%~nf.d
    call set out=%%~nf.tree
    echo Compiling ../results/test_code/!in! into ../results/test_code/!out!...
    ..\bin\compile "../results/test_code/!in!" -o "../results/optimized_outputs/!out!"
)
