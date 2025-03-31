#include <iostream>
#include <vector>
#include <mpi.h>

class SenseReversingBarrier {
private:
    int num_processes;  // Total number of MPI processes
    int rank;           // Rank of the current process

public:
    SenseReversingBarrier(int num_processes, int rank)
        : num_processes(num_processes), rank(rank) {}

    void wait() {
        // Use MPI_Barrier to synchronize all processes
        MPI_Barrier(MPI_COMM_WORLD);
    }
};

void worker(SenseReversingBarrier& barrier, int rank, const std::vector<int>& arr, int start, int end, int& local_sum) {
    local_sum = 0;
    // Calculate sum for the assigned chunk of the array
    for (int i = start; i < end; ++i) {
        local_sum += arr[i];
        std::cout << "Process " << rank << " processing element: " << arr[i] << std::endl;
    }

    // Synchronize with other processes
    barrier.wait();
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);  // Initialize MPI environment

    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);  // Get the total number of processes

    // Define the array and the number of threads (in this case, MPI processes)
    std::vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n = arr.size();

    // Calculate chunk size for each process
    int chunk_size = n / num_processes;
    int start = rank * chunk_size;
    int end = (rank == num_processes - 1) ? n : (rank + 1) * chunk_size;  // Handle the remainder for the last process

    int local_sum = 0;
    SenseReversingBarrier barrier(num_processes, rank);

    // Each process calculates its local sum
    worker(barrier, rank, arr, start, end, local_sum);

    // After the barrier, we reduce all the local sums into the root process
    int total_sum = 0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Process 0 prints the total sum
    if (rank == 0) {
        std::cout << "Total sum: " << total_sum << std::endl;
    }

    MPI_Finalize();  // Finalize the MPI environment
    return 0;
}
