#include "../include/matrix.hpp"
#include <thread>

Matrix::Matrix(int rows, int cols) : rows_(rows), cols_(cols), data_(rows*cols) {}

int Matrix::rows() const { return rows_; }
int Matrix::cols() const { return cols_; }

void Matrix::fillSingleThreaded(std::function<int(int,int)> filler) {
    for(int r=0; r<rows_; ++r)
        for(int c=0; c<cols_; ++c)
            data_[r*cols_ + c] = filler(r,c);
}

void Matrix::fillMultiThreaded(std::function<int(int,int)> filler, int numThreads) {
    auto worker = [this,&filler](int startRow,int endRow){
        for(int r=startRow; r<endRow; ++r)
            for(int c=0; c<cols_; ++c)
                data_[r*cols_ + c] = filler(r,c);
    };

    std::vector<std::thread> threads;
    int rowsPerThread = rows_ / numThreads;
    int extra = rows_ % numThreads;

    int currentRow = 0;
    for(int i=0; i<numThreads; ++i) {
        int endRow = currentRow + rowsPerThread + (i<extra ? 1 : 0);
        threads.emplace_back(worker, currentRow, endRow);
        currentRow = endRow;
    }

    for(auto& t : threads) t.join();
}
