#ifndef __ITERATIVE_IMPROVEMENT_HPP
#define __ITERATIVE_IMPROVEMENT_HPP

#include "pfspinstance.hpp"
#include "pfspsolution.hpp"

enum InitType {
	RANDOM = 0,
	SRZ = 1
};

enum ImprovementType {
	FIRST = 0,
	BEST = 1
};

enum NeighbourhoodType {
	TRANSPOSE = 0,
	EXCHANGE = 1,
	INSERT = 2
};


class IterativeImprovement
{
private:

public:
	IterativeImprovement(const PfspInstance & instance, PfspSolution & sol, InitType init, ImprovementType impvmnt, NeighbourhoodType neighbourhood);
};

#endif // __ITERATIVE_IMPROVEMENT_HPP