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

bool test_correctness(int num_processes, int times) {
    counter = 0;

    auto start = std::chrono::high_resolution_clock::now();

    int base = times / num_processes;
    int rem = times % num_processes;

    // Khởi tạo MPI
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int iters = base + (rank < rem ? 1 : 0);
    increment_mpi(iters);

    MPI_Barrier(MPI_COMM_WORLD);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    if (rank == 0) {
        bool result = (counter == num_processes * times);
        std::string pass_fail = result ? "Pass" : "Fail";
        std::cout << "Times: " << times << "\tProcesses: " << num_processes
                  << "\tExecution Time (ms): " << duration.count() * 1000
                  << "\tCounter: " << counter << "\t" << pass_fail << std::endl;
    }

    return counter == num_processes * times;
}

void run_test(int times, int num_processes) {
    bool result = test_correctness(num_processes, times);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    for (int times : {1000, 100000, 10000000}) {
        for (int num_processes : {1, 2, 4, 8}) {
            run_test(times, num_processes);
        }
    }

    MPI_Finalize();
    return 0;
}
