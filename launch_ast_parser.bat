@ECHO OFF
title Parsing all the syntax trees...
setlocal enabledelayedexpansion

if "%1" NEQ "/n" (
	echo Compiling the tree parser...
	g++ ast_parser.cpp -w -std=c++20 -o ast_parser
)

for %%f in (tokenizer_outputs/*.tokens) do (
    call set in=%%~nf.tokens
    call set out=%%~nf.tree
    echo Constructing syntax tree for tokenizer_outputs/!in! into ast_parser_outputs/!out!...
    ast_parser "tokenizer_outputs/!in!" -o "ast_parser_outputs/!out!"
)
