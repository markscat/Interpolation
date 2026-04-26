#pragma once

#include <vector>
#include <iostream>

// 若要切換成 vector，請取消下面這一行的註解
 #define method_vector 

#ifdef method_vector

class Matrix {
private:
    int rows;
    int cols;
    std::vector<std::vector<double>> data;

public:
    Matrix(int r, int c);
    void input();
    void load(const double* arr);

    void print() const;
    int getRows() const { return rows; }
    int getCols() const { return cols; }

    Matrix add(const Matrix& other) const;
    Matrix subtract(const Matrix& other) const;
    Matrix multiply(const Matrix& other) const;
};

#else

class Matrix {
private:
    int rows;
    int cols;
    double** data;

public:
    Matrix(int r, int c);
    ~Matrix();
    Matrix(const Matrix& other);

    void input();
    void load(const double* arr);

    void print() const;
    int getRows() const { return rows; }
    int getCols() const { return cols; }

    Matrix add(const Matrix& other) const;
    Matrix subtract(const Matrix& other) const;
    Matrix multiply(const Matrix& other) const;
};

#endif