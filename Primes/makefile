result: compile
	mpirun -np 1 exe
	
	mpirun -np 2 -hostfile hostlist --pernode exe
	
	# mpirun -np 4 -hostfile hostlist --pernode exe

	# mpirun -np 6 -hostfile hostlist --pernode exe
	
	mpirun -np 8 -hostfile hostlist --pernode exe
	
	mpirun -np 16 -hostfile hostlist -npernode 2 exe
	
	mpirun -np 32 -hostfile hostlist -npernode 4 exe

test1:
	mpicc -g parallelseive-1.c -lm
	mpirun -np 2 xterm -e gdb a.out

run16: compile
	mpirun -np 16 -hostfile hostlist -npernode 2 exe

run32: compile
	mpirun -np 32 -hostfile hostlist -npernode 4 exe

run1: compile
	mpirun -np 1 exe

gdb1: compile
	mpirun -np 1  xterm -e gdb exe

run4: compile
	mpirun -np 4 -hostfile hostlist --pernode exe

run2: compile
	mpirun -np 2 -hostfile hostlist --pernode exe

run8: compile
	mpirun -np 8 -hostfile hostlist --pernode exe

run : compile
	mpirun -np 4 exe

clean:
	rm exe output* *.o a.out

gdb : compile
	mpirun -np 4 xterm -e  gdb exe


compile: parallelseive.c
	mpicc -g parallelseive.c -lm -o exe
