# Sense-Reversing Barrier

## Update hostfile
```
localhost slots=2
root@128.199.107.103 slots=2
```

`root@128.199.107.103 slots=2` is OPTIONAL. Remove this line to run on local (shared memory) only.

## Run

```bash
mpic++ -o srb_mpi srb_mpi.cpp
mpirun -np 2 --hostfile hostfile ./srb_mpi
```