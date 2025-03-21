

mpic++ -o srb_mpi srb_mpi.cpp                                                                                                    INT ✘  00:31  
mpirun -np 2 -x SSH_AUTH_SOCK --hostfile hostfile ./srb_mpi     