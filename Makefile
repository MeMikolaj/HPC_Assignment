gccserial: main-serial.c stencil.c
	gcc -fopenmp main-serial.c stencil.c -o stencil-omp-gcc.exe -lm
gcccomplete: main-mpi.c stencil.c
	mpicc -fopenmp main-mpi.c stencil.c -o stencil-complete-gcc.exe -lm
iccserial: main-serial.c stencil.c
	icc -fopenmp main-serial.c stencil.c -o stencil-omp-icc.exe -lm
icccomplete: main-mpi.c stencil.c
	mpiicc -fopenmp main-mpi.c stencil.c -o stencil-complete-icc.exe -lm
