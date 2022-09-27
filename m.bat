@echo off
set OLDPATH=%PATH%
set PATH=..\tc\bin;%PATH%
del *.obj
del me.exe
del mex.exe
make -fmakefile.tc2 mex.exe
lzexe mex.exe
copy mex.exe me.exe
set PATH=%OLDPATH%
