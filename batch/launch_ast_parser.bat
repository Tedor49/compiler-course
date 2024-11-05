@ECHO OFF
title Parsing all the syntax trees...
setlocal enabledelayedexpansion

if "%1" NEQ "/n" (
	echo Compiling the tree parser...
	g++ "../src/partial/ast_parser.cpp" -w -std=c++20 -o "../bin/ast_parser"
)

for %%f in (../results/tokenizer_outputs/*.tokens) do (
    call set in=%%~nf.tokens
    call set out=%%~nf.tree
    echo Constructing syntax tree for ../results/tokenizer_outputs/!in! into ../results/ast_parser_outputs/!out!...
    ..\bin\ast_parser "../results/tokenizer_outputs/!in!" -o "../results/ast_parser_outputs/!out!"
)
