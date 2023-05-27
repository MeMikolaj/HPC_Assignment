# HPC_Assignment

High-Performance Computing.

Code to apply stencil filter (read from input file) to some 2D matrices (read from input file) and write the output to the file specified.

- The __main-serial.c__ contains a serial code implementation.
- The __stencil.c__ contains a function "stencil" used in main-serial.c and main-mpi.c that uses OpenMP to set the output \*float to correct values.
- The __main-mpi.c__ is an MPI implementation of the main code that devides the input file among X processes, uses Stencil with Y threads and writes to the output file.

- The __slurm-stencil.sh__ is a file to submit batch job to cluster as the input file can be too large to execute on the login node.
- The __Makefile__ produces correct exe file to submit them as a batch job.

- The __input.dat__ and __filter.dat__ are input and filter respectively. User also needs to provide the __output.dat__ file that the program will write to. It can be empty or not (will be overwritten).
