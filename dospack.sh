#!/bin/sh

cd ..
if [ -d dos ]
then
	echo dir dos already existing >&2
	exit 2
fi

rm -f mex.tgz
mkdir dos
cp -a /wintel/runready/tc dos
cp -a /wintel/runready/usrbin/lzexe.exe dos/tc/bin
mkdir dos/mex
for i in ed.h buffer.c cursor.c display.c files.c globargs.c help_c.eng line.c main.c random.c search.c term.c window.c word.c Makefile.TC2 mex.opt turboc.cfg m.bat
do
	test -f mex/$i || continue
	unix2dos <mex/$i >dos/mex/$i
done
tar cvzf dosmex.tgz dos
rm -rf dos

exit
