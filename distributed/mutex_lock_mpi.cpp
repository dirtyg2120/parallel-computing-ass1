#include <iostream>
#include <mpi.h>
#include <vector>
#include <chrono>

int counter = 0;

void increment_mpi(int times) {
    int local_counter = 0;

    for (int i = 0; i < times; ++i) {
        ++local_counter;
    }

    int global_counter = 0;
    MPI_Reduce(&local_counter, &global_counter, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (counter == 0) {
        counter = global_counter;
    }
}

void run_test_mpi(int times, int num_processes) {
    counter = 0;

    auto start = std::chrono::high_resolution_clock::now();

    int base = times / num_processes;
    int rem = times % num_processes;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int iters = base + (rank < rem ? 1 : 0);
    increment_mpi(iters);

    MPI_Barrier(MPI_COMM_WORLD);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    if (rank == 0) {
        std::cout << "Times: " << times << "\tProcesses: " << num_processes
                  << "\tExecution Time (ms): " << duration.count() * 1000
                  << "\tCounter: " << counter
                  << std::endl;
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    for (int times : {1000, 100000, 10000000}) {
        for (int processes : {1, 2, 4, 8}) {
            run_test_mpi(times, processes);
        }
    }

    MPI_Finalize();
    return 0;
}
