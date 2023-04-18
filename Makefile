

#CC = gcc
#CFLAGS = -Wall -Werror
# TODO : redo this Makefile in a better way

src/main		: clean src/main.o src/qlearning.o src/colored_output.o src/dfs.o src/functions.o src/mazeEnv.o
	gcc -I include/ -g -o src/main.x src/main.o src/qlearning.o src/colored_output.o src/dfs.o src/functions.o src/mazeEnv.o -Wall


src/main.o      : src/main.c include/main.h
	gcc -I include/ -g -o src/main.o -c src/main.c -Wall

src/qlearning.o	: src/qlearning.c include/qlearning.h
	gcc -I include/ -g -o src/qlearning.o -c src/qlearning.c -Wall

src/colored_output.o : src/colored_output.c include/colored_output.h
	gcc -I include/ -g -o src/colored_output.o -c src/colored_output.c -Wall

src/dfs.o       : src/dfs.c include/dfs.h include/mazeEnv.h
	gcc -I include/ -g -o src/dfs.o -c src/dfs.c -Wall

src/functions.o : src/functions.c include/functions.h
	gcc -I include/ -g -o src/functions.o -c src/functions.c -Wall

src/mazeEnv.o   : src/mazeEnv.c include/mazeEnv.h
	gcc -I include/ -g -o src/mazeEnv.o -c src/mazeEnv.c -Wall

clean:
	find . -type f -name "*.o" -delete

realclean: clean
	find . -type f -name "*.x" -delete










