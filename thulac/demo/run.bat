@echo off
%~d0
cd /d %~dp0
echo START PROGRAM...
::reference: https://github.com/thunlp/THULAC
:: use the models/cws_model.bin
Thulac.exe -model_dir models -seg_only -input test.txt -output outputfile.txt 
Thulac.exe -model_dir models -deli _ -input test.txt -output outputfile2.txt 
echo EXIT PROGRAM.
pause > nul