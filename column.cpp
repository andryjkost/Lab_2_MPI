//
// Created by andrej on 20.03.2023.
//
#include "utilities.h"


int main(int argc, char** argv) {
    int rank, size;

    double starttime, endtime;

    string output_file = "data/column_time.csv";

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank);

    int local_n = SIZE_MATRIX / size;
    Matrix matrix_a, matrix_b, result_matrix;

    // Инициализация матриц
    if (rank == MASTER_PROC_RANK) {
        matrix_a = initialize_matrix(SIZE_MATRIX, -1);
        matrix_b = initialize_matrix(SIZE_MATRIX, local_n);

        cout << "Инициализация первой матрицы matrix_a:\n";
        print_matrix(matrix_a, -1);
        cout << "Инициализация второй матрицы:\n";
        print_matrix(matrix_b, local_n);
    }

    starttime = MPI_Wtime();
    MPI_Scatter(matrix_b.arr, SIZE_MATRIX * local_n, MPI_INT, matrix_b.arr,
                SIZE_MATRIX * local_n, MPI_INT, MASTER_PROC_RANK, MPI_COMM_WORLD);
    MPI_Bcast(matrix_a.arr, SIZE_MATRIX * SIZE_MATRIX, MPI_INT, MASTER_PROC_RANK,
              MPI_COMM_WORLD);

    // Умножение
    for (int i = 0; i < SIZE_MATRIX; i++) {
        for (int j = 0; j < SIZE_MATRIX/local_n; j++) {
            int sum = 0;
            for (int k = 0; k < SIZE_MATRIX; k++) {
                sum += matrix_a.arr[i][k] * matrix_b.arr[k][j];
            }
            result_matrix.arr[i][j] = sum;
        }
    }

    // Сбор результатов
    MPI_Gather(result_matrix.arr, SIZE_MATRIX * local_n, MPI_INT, result_matrix.arr,
               SIZE_MATRIX * local_n, MPI_INT, MASTER_PROC_RANK, MPI_COMM_WORLD);

    endtime   = MPI_Wtime();
    if (rank == MASTER_PROC_RANK) {
        cout << "Результат:\n";
        print_matrix(result_matrix, -1);
    }

    MPI_Finalize();

    cout << "Время: " << to_string(endtime-starttime) << '\n';
    string output_text = "Размер матрицы: " + to_string(SIZE_MATRIX) +
                         " Кол-во процессов: " + to_string(size) +
                         " Время: " + to_string(endtime-starttime)
                         + "\n";
    write_to_file(output_file, output_text);
    return 0;
}

