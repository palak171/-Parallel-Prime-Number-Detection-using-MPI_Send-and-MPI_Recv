#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 100

// Function to check if a number is prime
int is_prime(int num) {
    if (num < 2) return 0;
    for (int i = 2; i * i <= num; i++)
        if (num % i == 0) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    int rank, size, num;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {  // Master process
        int count = 2; // Start checking from 2
        int active_workers = size - 1;

        while (active_workers > 0) {
            int received;
            MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            if (received > 0) // Prime number
                printf("%d is prime\n", received);

            if (count < MAX) {
                MPI_Send(&count, 1, MPI_INT, received, 0, MPI_COMM_WORLD);
                count++;
            } else {
                int stop = -1;
                MPI_Send(&stop, 1, MPI_INT, received, 0, MPI_COMM_WORLD);
                active_workers--;
            }
        }
    } else {  // Worker processes
        while (1) {
            int num_to_test;
            MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&num_to_test, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (num_to_test == -1) break;
            int result = is_prime(num_to_test) ? num_to_test : -num_to_test;
            MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
