#!/bin/bash -l

#SBATCH -D ./
#SBATCH --export=ALL
#SBATCH -p course,nodes -t 3 

# load modules
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin


maxThreads=${SLURM_CPUS_PER_TASK:-1} # if '-c' not used then default to 1
export OMP_NUM_THREADS=$maxThreads
export numMPI=${SLURM_NTASKS:-1}

echo gcc stencil OMP
stencil-omp-gcc.exe input.dat kernel_5.dat o1.dat

echo gcc stencil MPI
mpirun -np ${SLURM_NTASKS:-1} stencil-complete-gcc.exe input.dat kernel_5.dat o2.dat

echo intel stencil OMP
stencil-omp-icc.exe input.dat kernel_5.dat o3.dat

echo intel stencil MPI
mpirun -np ${SLURM_NTASKS:-1} ./stencil-complete-icc.exe input.dat kernel_5.dat o4.dat
