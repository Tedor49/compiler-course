@ECHO OFF
title Checking all the syntax trees for correctness...
setlocal enabledelayedexpansion

if "%1" NEQ "/n" (
	echo Compiling the analyzers...
	g++ ../src/partial/analyze.cpp -w -std=c++20 -o ../bin/analyze
)

for %%f in (../results/ast_parser_outputs/*.tree) do (
	call set code=%%~nf.tree
	echo Checking ../results/ast_parser_outputs/!code! for corretness... 
	..\bin\analyze "../results/ast_parser_outputs/!code!" -v
)