# Makfile for Project 8

SRC = hake.c
LIB = macro.c names.c cmpsc311.c
INC = macro.h names.h cmpsc311.h

mac : ${SRC} ${LIB}
	gcc -std=c99 -Wall -Wextra -o hake ${SRC} ${LIB} ${INC}
