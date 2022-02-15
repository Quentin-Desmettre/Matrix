#include "Matrix.hpp"

#include <chrono>
#include <iostream>

void naive(void)
{
    auto start = std::chrono::high_resolution_clock::now();
    int **A = (int **)malloc(sizeof(int*) * 1500);
    int **B = (int **)malloc(sizeof(int*) * 1500);
    int **C = (int **)malloc(sizeof(int*) * 1500);

    for (int i = 0; i < 1500; i++) {
        A[i] = (int *)malloc(sizeof(int) * 1500);
        B[i] = (int *)malloc(sizeof(int) * 1500);
        C[i] = (int *)malloc(sizeof(int) * 1500);
        for (int j = 0; j < 1500; j++) {
            A[i][j] = 2;
            B[i][j] = -3;
            C[i][j] = 0;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> float_ms = end - start;
    std::cout << "Exec time: " << float_ms.count() << " milliseconds" << std::endl;
}

void opti(void)
{
    int const size = 1500 * 1500;
    int *A2 = (int *)malloc(sizeof(int) * size);
    int *B2 = (int *)malloc(sizeof(int) * size);
    int *C2 = (int *)malloc(sizeof(int) * size);

    for (int i = 0; i < size; i++) {
        A2[i] = 2;
        B2[i] = -3;
        C2[i] = 0;
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1500; i++) {
        for (int k = 0; k < 1500; k++)
            for (int j = 0; j < 1500; j++) {
                C2[i * 1500 + j] += A2[i * 1500 + k] * B2[k * 1500 + j];
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> float_ms = end - start;
    std::cout << "Exec time: " << float_ms.count() << " milliseconds" << std::endl;
}

void setRand(cppm::Matrix<int> &m)
{
    for (int i = 0; i < m.getSize()[0]; i++) {
        for (int j = 0; j < m.getSize()[1]; j++) {
            m.at(i, j) = rand() % 10 - 5;
        }
    }
}

void prntMatrix(cppm::Matrix<int> const& m)
{
    for (cppm::uint64 i = 0; i < m.getSize()[0]; i++) {
        for (cppm::uint64 j = 0; j < m.getSize()[1]; j++) {
            printf("%d ", m.at(i, j));
        }
        printf("\n");
    }
}

void classes(void)
{
    cppm::Matrix<int> m1(2, 5, true, 2);
    cppm::Matrix<int> m2(5, 1, true, -3);

    setRand(m1);
    setRand(m2);

    prntMatrix(m1);
    printf("\n");
    prntMatrix(m2);
    printf("\n");
    //auto start = std::chrono::high_resolution_clock::now();
    //prntMatrix(m3);
    cppm::Matrix<int> m3 = m1 * m2;
    printf("M3 is init\n");
    prntMatrix(m3);
    cppm::Matrix<int> t = m3;
    t = m1;
    t = m2;
    t = m3;
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> float_ms = end - start;
    // std::cout << "Exec time: " << float_ms.count() << " milliseconds" << std::endl;
}

int main(void)
{
    classes();
    return 0;
}
