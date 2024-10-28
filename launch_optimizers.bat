@ECHO OFF
title Checking all the syntax trees for correctness...
setlocal enabledelayedexpansion

echo Compiling the optimizers...
for %%f in (optimizers/*.cpp) do (
	call set in=%%~nf.cpp
	call set out=%%~nf
	echo Compiling optimizers/!in!...
	g++ optimizers/!in! -w -std=c++20 -o optimizers_comp/!out!
)

for %%f in (ast_parser_outputs/*.tree) do (
	call set code=%%~nf.tree
	echo Optimizing ast_parser_outputs/!code!... 
	
	call set i=0
	copy /Y /a "ast_parser_outputs\!code!" /a "0.opt" /a >NUL
	
	for %%f in (optimizers_comp/*.exe) do (
		call set checker=%%~nf
		call set /A newi=!i!+1
		
		echo Optimizing with !checker!...
		
		optimizers_comp\!checker! "!i!.opt" -o "!newi!.opt"
		del "!i!.opt"
		
		call set i=!newi!
	)
	copy /Y /a "!i!.opt" /a "optimized_outputs\!code!" /a >NUL
	del "!i!.opt" 
)