run4: exe
	mpirun -np 4 ./exe

run1: exe
	mpirun -np 1 ./exe

run2: exe
	mpirun -np 2 ./exe

gdb1: exe
	mpirun -np 2 xterm -e gdb exe

exe: hash.o parallel.o hash.h
	mpicc hash.o parallel.o -o exe

parallel.o: parallel.c hash.h
	mpicc -c -g parallel.c

hash.o: hash.c hash.h
	mpicc -g -c hash.c

compileall: hash.c hash.h
	gcc -g hash.c -o exe

clean: 
	rm *.o exe