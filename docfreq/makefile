parallel: hash.o parallel.o hash.h
	mpicc hash.o parallel.o -o exe

parallel.o: parallel.c hash.h
	mpicc -c parallel.c

hash.o: hash.c hash.h
	mpicc -c hash.c

compileall: hash.c hash.h
	gcc -g hash.c -o exe