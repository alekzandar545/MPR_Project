#pragma once
#include <vector>
#include <thread>
#include <functional>
#include <sstream>

class Matrix {
public:
    Matrix(int rows, int cols);
    
    void fillSingleThreaded();
    void fillMultiThreaded(int numThreads);
    std::string previewMatrix();

    int rows() const;
    int cols() const;

private:
    std::vector<int> data_;
    int rows_;
    int cols_;
};
