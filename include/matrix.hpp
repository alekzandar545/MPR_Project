#pragma once
#include <vector>
#include <thread>
#include <functional>

class Matrix {
public:
    Matrix(int rows, int cols);
    
    void fillSingleThreaded(std::function<int(int,int)> filler);
    void fillMultiThreaded(std::function<int(int,int)> filler, int numThreads);

    int rows() const;
    int cols() const;

private:
    std::vector<int> data_;
    int rows_;
    int cols_;
};
