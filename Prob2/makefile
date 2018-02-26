runall: exe run1 run2 run4 run8 run16 run32
	
runtest: exe
	mpirun -np 3 -hostfile hostlist --pernode exe

	mpirun -np 6 -hostfile hostlist -npernode 2 exe

	mpirun -np 9 -hostfile hostlist -npernode 3 exe

	mpirun -np 12 -hostfile hostlist -npernode 4 exe

run16: exe
	mpirun -np 16 -hostfile hostlist -npernode 4 exe

run32: exe
	mpirun -np 32 -hostfile hostlist -npernode 8 exe

run1: exe
	mpirun -np 1 exe

gdb1: exe
	mpirun -np 1  xterm -e gdb exe

run4: exe
	mpirun -np 4 -hostfile hostlist --pernode exe

run2: exe
	mpirun -np 2 -hostfile hostlist --pernode exe

run8: exe
	mpirun -np 8 -hostfile hostlist -npernode 2 exe


# run4: exe
# 	mpirun -np 4 ./exe

# run1: exe
# 	mpirun -np 1 ./exe

# run2: exe
# 	mpirun -np 2 ./exe

gdb: exe
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
	rm *.o exe output*
