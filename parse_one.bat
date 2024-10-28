@ECHO OFF
title Parsing all the syntax trees...
setlocal enabledelayedexpansion

echo Compiling the tree parser...
g++ ast_parser.cpp -w -std=c++20 -o ast_parser

ast_parser "tokenizer_outputs/1 (2).tokens"