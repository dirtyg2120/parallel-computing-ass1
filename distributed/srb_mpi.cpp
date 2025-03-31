#include <iostream>
#include <vector>
#include <mpi.h>

class SenseReversingBarrier {
private:
    int num_processes;
    int rank;

public:
    SenseReversingBarrier(int num_processes, int rank)
        : num_processes(num_processes), rank(rank) {}

    void wait() {
        MPI_Barrier(MPI_COMM_WORLD);
    }
};

void worker(SenseReversingBarrier& barrier, int rank, const std::vector<int>& arr, int start, int end, int& local_sum) {
    local_sum = 0;
    for (int i = start; i < end; ++i) {
        local_sum += arr[i];
        std::cout << "Process " << rank << " processing element: " << arr[i] << std::endl;
    }

    barrier.wait();
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes); 

    std::vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n = arr.size();

    int chunk_size = n / num_processes;
    int start = rank * chunk_size;
    int end = (rank == num_processes - 1) ? n : (rank + 1) * chunk_size;

    int local_sum = 0;
    SenseReversingBarrier barrier(num_processes, rank);

    worker(barrier, rank, arr, start, end, local_sum);

    int total_sum = 0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Total sum: " << total_sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
