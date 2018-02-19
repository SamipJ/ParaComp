run : compile
	mpirun -np 4 exe

gdb : compile
	mpirun -np 4 xterm -e gdb exe


compile: parallelseive.c
	mpicc -g parallelseive.c -lm -o exe
