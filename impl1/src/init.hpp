#ifndef __INIT_HPP
#define __INIT_HPP

#include <vector>
#include "pfspinstance.hpp"
#include "pfspsolution.hpp"

void orderedSolution(const PfspInstance & instance, PfspSolution & sol);
void randomPermutation(const PfspInstance & instance, PfspSolution & sol);
void simplifiedRZ(const PfspInstance & instance, PfspSolution & sol);

#endif // __INIT_HPP