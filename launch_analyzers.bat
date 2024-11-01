@ECHO OFF
title Checking all the syntax trees for correctness...
setlocal enabledelayedexpansion

if "%1" NEQ "/n" (
	echo Compiling the analyzers...
	for %%f in (analyzers/*.cpp) do (
		call set in=%%~nf.cpp
		call set out=%%~nf
		echo Compiling analyzers/!in!...
		g++ analyzers/!in! -w -std=c++20 -o analyzers_comp/!out!
	)
)

for %%f in (ast_parser_outputs/*.tree) do (
	call set code=%%~nf.tree
	echo Checking ast_parser_outputs/!code! for corretness... 
	for %%f in (analyzers_comp/*.exe) do (
		call set checker=%%~nf
		echo Checking using !checker!...
		
		analyzers_comp\!checker! "ast_parser_outputs/!code!"
		if %ERRORLEVEL% NEQ 0 echo Error
	)
)