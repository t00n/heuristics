#ifndef __PFSPSOLUTION_HPP
#define __PFSPSOLUTION_HPP

#include <iostream>
#include <vector>

template<typename T>
void printVector(std::vector<T> const & v);

typedef std::vector<int> PfspSolution;

void printSolution(PfspSolution const & sol);

#endif // __PFSPSOLUTION_HPP