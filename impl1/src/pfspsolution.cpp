#include "pfspsolution.hpp"

#include <vector>
#include <iostream>

template<typename T>
void printVector(std::vector<T> const & v) {
    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void printSolution(PfspSolution const & sol) {
    PfspSolution v(sol);
    for (auto it = v.begin(); it != v.end(); ++it) {
        (*it)++;
    }
    printVector(v);
}