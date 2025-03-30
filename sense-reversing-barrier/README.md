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
g++ -fopenmp -o openmp_barrier openmp_barrier.cpp
./openmp_barrier
```

Pthreads Version:
```bash
g++ -pthread -o pthread_barrier pthread_barrier.cpp
./pthread_barrier
```

Normal C++ Version:
```bash
g++ -std=c++11 -o normal_barrier normal_barrier.cpp
./normal_barrier
```

C++ with Exponential Backoff Version:
```bash
g++ -std=c++11 -o srb_expo_backoff srb_expo_backoff.cpp
./srb_expo_backoff
```

MPI version:
```bash
mpic++ -o srb_mpi srb_mpi.cpp
mpirun -np 2 --hostfile hostfile ./srb_mpi
```

