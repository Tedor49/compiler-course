@ECHO OFF
setlocal enabledelayedexpansion

echo Compiling the tree executor...
g++ ../src/partial/execute_tree.cpp -w -std=c++20 -o ../bin/execute_tree