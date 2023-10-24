#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int array_size = 2000;
int threadsNos[] = {1, 2, 4, 6, 8};

void multiply(int **A, int **B, int **R, int begin, int skip) {
    for (int i = begin; i < array_size; i += skip) {
        for (int j = 0; j < array_size; j++) {
            for (int k = 0; k < array_size; k++) {
                R[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void multiplyTransposed(int **A, int **B, int **R, int begin, int skip) {
    for (int i = begin; i < array_size; i += skip) {
        for (int j = 0; j < array_size; j++) {
            for (int k = 0; k < array_size; k++) {
                R[i][j] += A[i][k] * B[j][k];
            }
        }
    }
}

void transpose(int **src, int **dst) {
    for (int i = 0; i < array_size; i++) {
        for (int j = 0; j < array_size; j++) {
            dst[i][j] = src[j][i];
        }
    }
}

void initArr(int **arr) {

    for (int i = 0; i < array_size; i++) {
        arr[i] = new int[array_size];
    }

    for (int i = 0; i < array_size; i++) {
        for (int j = 0; j < array_size; j++) {
            arr[i][j] = j * i;
        }
    }
}

void deleteArr(int **arr) {
    for (int i = 0; i < array_size; i++) {
        delete[] arr[i];
    }

    delete[] arr;
}

int main() {

    int **A = new int *[array_size];
    int **B = new int *[array_size];
    int **BT = new int *[array_size];
    int **R = new int *[array_size];

    initArr(A);
    initArr(B);
    initArr(BT);
    initArr(R);

    transpose(A, B);


    for (int threads_number: threadsNos) {

        cout << "array size: " << array_size << " threads " << threads_number << endl;

        thread threads[threads_number];
        steady_clock::time_point start = high_resolution_clock::now();
        for (int i = 0; i < threads_number; i++) {
            threads[i] = thread(multiply, A, B, R, i, threads_number);
        }

        for (auto &thread: threads) {
            thread.join();
        }
        steady_clock::time_point stop = high_resolution_clock::now();

        auto time = duration_cast<std::chrono::milliseconds>(stop - start);
        cout << "Standard multiply " << time.count() << std::endl;

        start = high_resolution_clock::now();
        for (int i = 0; i < threads_number; i++) {
            threads[i] = std::thread(multiplyTransposed, A, B, R, i, threads_number);
        }

        for (auto &thread: threads) {
            thread.join();
        }
        stop = high_resolution_clock::now();

        time = duration_cast<std::chrono::milliseconds>(stop - start);
        cout << "Transposed multiply " << time.count() << std::endl;
    }

    deleteArr(A);
    deleteArr(B);
    deleteArr(BT);
    deleteArr(R);

    return 0;
}
