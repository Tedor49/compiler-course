@ECHO OFF
setlocal enabledelayedexpansion

echo Compiling the compiler...
g++ ../src/compile.cpp -w -std=c++20 -o ../bin/compile