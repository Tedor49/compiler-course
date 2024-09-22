@ECHO OFF
title Tokenizing all the test codes...
setlocal enabledelayedexpansion

echo Compiling the tokenizer...
g++ tokenizer.cpp -w -std=c++20 -o tokenizer
echo Compilation successful!

for %%f in (test_code/*.d) do (
    call set in=%%~nf.d
    call set out=%%~nf.tokens
    echo Tokenizing test_code/!in! to tokenizer_outputs/!out!...
    tokenizer "test_code/!in!" -o "tokenizer_outputs/!out!"
)
