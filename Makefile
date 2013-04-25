# Makfile for Project 8

SRC = hake.c
LIB = macro.c names.c cmpsc311.c lister.c
INC = macro.h names.h cmpsc311.h lister.h

# Solaris
# OPT = -D_XOPEN_SOURCE=600
# Linux
# OPT = -D_XOPEN_SOURCE=700 -DMTIME=1
# Mac OS X
OPT = -DMTIME=2

pr8 : ${SRC} ${LIB} ${INC}
	gcc -std=c99 -Wall -Wextra ${OPT} -o hake ${SRC} ${LIB} ${INC}

clean:
	rm pr8
