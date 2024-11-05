@ECHO OFF
title Optimizing all the syntax trees...
setlocal enabledelayedexpansion

if "%1" NEQ "/n" (
	echo Compiling the optimizers...
	g++ ../src/partial/optimize.cpp -w -std=c++20 -o ../bin/optimize
)

for %%f in (../results/ast_parser_outputs/*.tree) do (
	call set code=%%~nf.tree
	echo Optimizing ../results/ast_parser_outputs/!code!... 
	..\bin\optimize "../results/ast_parser_outputs/!code!" -v -o "../results/optimized_outputs/!code!"
)