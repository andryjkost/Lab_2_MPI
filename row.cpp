//
// Created by andrej on 20.03.2023.
//
#include "utilities.h"


int main(int argc, char **argv) {
    int rank, size;

    double start_t, end_t;
    struct timeval t;

    string output_file = "data/row_time.csv";

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank);

    int local_n = SIZE_MATRIX / size;
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

    gettimeofday(&t, NULL);
    start_t = t.tv_sec + t.tv_usec/1000000.0;

    MPI_Scatter(matrix_a.arr, SIZE_MATRIX * local_n, MPI_INT,
                matrix_a.arr[rank * local_n], SIZE_MATRIX * local_n, MPI_INT,
                MASTER_PROC_RANK, MPI_COMM_WORLD);
    MPI_Bcast(matrix_b.arr, SIZE_MATRIX * SIZE_MATRIX, MPI_INT,
              MASTER_PROC_RANK, MPI_COMM_WORLD);

    // Умножение
    for (int i = 0; i < SIZE_MATRIX / local_n; i++) {
        for (int j = 0; j < SIZE_MATRIX; j++){
            int sum = 0;
            for (int k = 0; k < SIZE_MATRIX; k++) {
                sum += matrix_a.arr[i + rank * local_n][k] * matrix_b.arr[k][j];
            }
            result_matrix.arr[i][j] = sum;
        }
    }

    // Сбор результов
    MPI_Gather(result_matrix.arr, SIZE_MATRIX * local_n, MPI_INT, result_matrix.arr,
               SIZE_MATRIX * local_n, MPI_INT, MASTER_PROC_RANK, MPI_COMM_WORLD);

    gettimeofday(&t, NULL);
    end_t = t.tv_sec + t.tv_usec/1000000.0;

    if (rank == MASTER_PROC_RANK) {
        cout << "Результат:\n";
        print_matrix(result_matrix, -1);
    }

    MPI_Finalize();

    cout << "Время: " << to_string(end_t - start_t) << '\n';
    string output_text = "Размер матрицы: " + to_string(SIZE_MATRIX) +
                         " Кол-во процессов: " + to_string(size) +
                         " Время: " + to_string(end_t - start_t)
                         + "\n";
    write_to_file(output_file, output_text);
    return 0;
}

