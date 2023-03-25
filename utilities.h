//
// Created by andrej on 23.03.2023.
//

#ifndef MPI_UTILITIES_H
#define MPI_UTILITIES_H

# include <sys/time.h>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

const int SIZE_MATRIX = 4;      // Размер матрицы
const int BLOCK_SIZE = 1; // Размер блока в матрицы
const int MASTER_PROC_RANK = 0; // Ранг главного процесса

struct Matrix {
    int arr[SIZE_MATRIX][SIZE_MATRIX];
};

Matrix initialize_matrix(int, int);

void print_matrix(Matrix, int);

void write_to_file(string, string);


Matrix initialize_matrix(int N, int local_n) {
    Matrix matrix;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (local_n != -1) {
                matrix.arr[i][j / local_n] = rand() % 10;
            } else {
                matrix.arr[i][j] = rand() % 10 + 1;
            }
        }
    }
    return matrix;
}


void print_matrix(Matrix matrix, int local_n) {
    for (int i = 0; i < SIZE_MATRIX; i++) {
        for (int j = 0; j < SIZE_MATRIX; j++) {
            if (local_n != -1) {
                cout << matrix.arr[i][j / local_n] << " ";
            } else {
                cout << matrix.arr[i][j] << " ";
            }
        }
        cout << endl;
    }
}


void write_to_file(string filename, string output_text) {
    ofstream output;
    output.open(filename, ios_base::app);
    output << output_text;
    output.close();
}

#endif //MPI_UTILITIES_H
