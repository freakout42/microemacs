@echo off
rem dosbox mount c m:\p\; c:; cd mex; m.bat
set OLDPATH=%PATH%
set PATH=..\tc\bin;%PATH%
del *.obj
del me.exe
del mex.exe
make -fmakefile.tc2 mex.exe
rem lzexe mex.exe
rem copy mex.exe me.exe
set PATH=%OLDPATH%
