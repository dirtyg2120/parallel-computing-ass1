# Sense-Reversing Barrier

## Update hostfile
```
localhost slots=2
root@128.199.71.102 slots=2
root@159.223.51.143 slots=2
```

`128.199.71.102 slots=2` is OPTIONAL. Remove this line to run on local (shared memory) only.

## Run

OpenMP Version:
```bash
g++ -o srb_openmp srb_openmp.cpp -fopenmp
./srb_openmp
```

Pthreads Version:
```bash
g++ -o srb_pthreads srb_pthreads.cpp -pthread 
./srb_pthreads
```

Normal C++ Version:
```bash
g++ -o srb srb.cpp -pthread
./srb
```

MPI version:
```bash
mpic++ -o srb_mpi srb_mpi.cpp
mpirun -np 6 --hostfile hostfile --allow-run-as-root ./srb_mpi
```

