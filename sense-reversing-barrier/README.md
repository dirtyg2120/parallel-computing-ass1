# Sense-Reversing Barrier

## Update hostfile
```
localhost slots=2
128.199.71.102 slots=2
```

`128.199.71.102 slots=2` is OPTIONAL. Remove this line to run on local (shared memory) only.

## Run

OpenMP Version:
```bash
g++ -fopenmp -o srb_openmp srb_openmp.cpp
./srb_openmp
```

Pthreads Version:
```bash
g++ -pthread -o srb_pthreads srb_pthreads.cpp
./srb_pthreads
```

Normal C++ Version:
```bash
g++ -std=c++11 -o srb srb.cpp
./srb
```

C++ with Exponential Backoff Version:
```bash
g++ -std=c++11 -o srb_expo_backoff srb_expo_backoff.cpp
./srb_expo_backoff
```

MPI version:
```bash
mpic++ -o srb_mpi srb_mpi.cpp
mpirun -np 2 --hostfile hostfile --allow-run-as-root ./srb_mpi
```

