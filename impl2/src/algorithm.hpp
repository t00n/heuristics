#ifndef __ALGORITHM_HPP
#define __ALGORITHM_HPP

#include <ctime>

#include "pfspinstance.hpp"
#include "pfspsolution.hpp"

class Algorithm {
	protected:
		PfspInstance instance;
		PfspSolution solution;
		std::clock_t timeout;

	public:
		Algorithm(const PfspInstance & instance, std::clock_t timeout);

		virtual void solve() = 0;

		PfspSolution getSolution();
};

PfspSolution randomSolution(const PfspInstance & instance);

void orderedSolution(const PfspInstance & instance, PfspSolution & sol);

void insert(PfspSolution & solution, size_t i, size_t j);

PfspSolution initLR(const PfspInstance &instance, unsigned int x);

PfspSolution RZ(const PfspInstance & instance, const PfspSolution& solution);

PfspSolution iRZ(const PfspInstance & instance, const PfspSolution& solution);

PfspSolution destruction_construction(const PfspInstance & instance, const PfspSolution & solution, unsigned int d);

int computeTemperature(int lambda, const PfspInstance & instance);

#endif // __ALGORITHM_HPP