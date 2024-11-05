@ECHO OFF
title Tokenizing all the test codes...
setlocal enabledelayedexpansion

if "%1" NEQ "/n" (
	echo Compiling the tokenizer...
	g++ "../src/partial/tokenize.cpp" -w -std=c++20 -o "../bin/tokenize"
)

for %%f in (../results/test_code/*.d) do (
    call set in=%%~nf.d
    call set out=%%~nf.tokens
    echo Tokenizing ../results/test_code/!in! to ../results/tokenizer_outputs/!out!...
    ..\bin\tokenize "../results/test_code/!in!" -o "../results/tokenizer_outputs/!out!"
)
