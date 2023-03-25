//
// Created by andrej on 20.03.2023.
//
#include "utilities.h"


int main(int argc, char **argv) {
    int rank, size;

    double starttime, endtime;

    string output_file = "data/block_time.csv";

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank);

    int local_n = SIZE_MATRIX / BLOCK_SIZE;
    Matrix matrix_a, matrix_b, result_matrix;

    // Инициализация матриц
    if (rank == MASTER_PROC_RANK) {
        matrix_a = initialize_matrix(SIZE_MATRIX, -1);
        matrix_b = initialize_matrix(SIZE_MATRIX, -1);

        cout << "Инициализация первой матрицы matrix_a:\n";
        print_matrix(matrix_a, -1);
        cout << "Инициализация второй матрицы:\n";
        print_matrix(matrix_b, local_n);
    }


    starttime = MPI_Wtime();
    MPI_Datatype block;
    MPI_Type_vector(BLOCK_SIZE, BLOCK_SIZE, SIZE_MATRIX, MPI_INT, &block);
    MPI_Type_commit(&block);
    MPI_Scatter(matrix_a.arr, 1, block, matrix_a.arr[rank / local_n * BLOCK_SIZE],
                BLOCK_SIZE * SIZE_MATRIX, MPI_INT,MASTER_PROC_RANK, MPI_COMM_WORLD);
    MPI_Bcast(matrix_b.arr, SIZE_MATRIX * SIZE_MATRIX, MPI_INT, MASTER_PROC_RANK,
              MPI_COMM_WORLD);

    // Умножение
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            int sum = 0;
            for (int k = 0; k < SIZE_MATRIX; k++) {
                sum += matrix_a.arr[i + rank / local_n * BLOCK_SIZE][k] *
                       matrix_b.arr[k][j + (rank % local_n) * BLOCK_SIZE];
            }
            result_matrix.arr[i + rank / local_n * BLOCK_SIZE][j + (rank % local_n) * BLOCK_SIZE] = sum;
        }
    }

    // Сбор результатов
    MPI_Gather(result_matrix.arr, BLOCK_SIZE * SIZE_MATRIX, MPI_INT, result_matrix.arr,
               BLOCK_SIZE * SIZE_MATRIX,MPI_INT, MASTER_PROC_RANK, MPI_COMM_WORLD);


    endtime   = MPI_Wtime();
    if (rank == MASTER_PROC_RANK) {
        cout << "Результат:\n";
        print_matrix(result_matrix, -1);
    }

    MPI_Finalize();


    cout << "Время: " << to_string(endtime-starttime) << '\n';
    string output_text = "Размер матрицы: " + to_string(SIZE_MATRIX) +
                         " Размер блока:" + to_string(BLOCK_SIZE) +
                         " Кол-во процессов: " + to_string(size) +
                         " Время: " + to_string(endtime-starttime)
                         + "\n";
    write_to_file(output_file, output_text);
    return 0;
}