@ECHO OFF
title Parsing all the syntax trees...
setlocal enabledelayedexpansion

echo Compiling the printer...
g++ print_tree.cpp -w -std=c++20 -o print_tree

for %%f in (ast_parser_outputs/*.tree) do (
    call set in=%%~nf.tree
    print_tree "ast_parser_outputs/!in!"
)
