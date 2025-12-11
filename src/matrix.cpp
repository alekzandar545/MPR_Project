#include "../include/matrix.hpp"

constexpr int DEFAULT_FILL = 10;

Matrix::Matrix(int rows, int cols) : rows_(rows), cols_(cols), data_(rows*cols) {}

int Matrix::rows() const { return rows_; }
int Matrix::cols() const { return cols_; }

void Matrix::fillSingleThreaded() {
    for(int r=0; r<rows_; ++r)
        for(int c=0; c<cols_; ++c)
            data_[r*cols_ + c] = DEFAULT_FILL;
}

void Matrix::fillMultiThreaded(int numThreads) {
    auto worker = [this](int startRow,int endRow,int threadIndex){
        int value = DEFAULT_FILL * (threadIndex + 1);
        for(int r=startRow; r<endRow; ++r)
            for(int c=0; c<cols_; ++c)
                data_[r*cols_ + c] = value;
    };

    std::vector<std::thread> threads;
    int rowsPerThread = rows_ / numThreads;
    int extra = rows_ % numThreads;

    int currentRow = 0;
    for(int i=0; i<numThreads; ++i) {
        int endRow = currentRow + rowsPerThread + (i<extra ? 1 : 0);
        threads.emplace_back(worker, currentRow, endRow, i);
        currentRow = endRow;
    }

    for(auto& t : threads) t.join();
}

std::string Matrix::previewMatrix() {
    std::ostringstream out;

    if (rows_ * cols_ < 100) {
        for(int r = 0; r < rows_; ++r) {
            for(int c = 0; c < cols_; ++c) {
                out << data_[r*cols_ + c] << " ";
            }
            out << "\n";
        }
    } else {
        int size = rows_ * cols_;
        int middle = size / 2;

        out << "First 10: ";
        for(size_t i = 0; i < 10; i++) out << data_[i] << " ";
        out << "\n";

        out << "Middle 10: ";
        for(size_t i = middle-5; i <= middle+5; i++) out << data_[i] << " ";
        out << "\n";

        out << "Last 10: ";
        for(size_t i = size-10; i < size; i++) out << data_[i] << " ";
        out << "\n";
    }

    return out.str();
}

