#ifndef __ALGORITHM_HPP
#define __ALGORITHM_HPP

#include "pfspinstance.hpp"
#include "pfspsolution.hpp"

class Algorithm {
	protected:
		PfspInstance instance;
		PfspSolution solution;

	public:
		Algorithm(const PfspInstance & instance);

		virtual void solve() = 0;

		PfspSolution getSolution();
};

#endif // __ALGORITHM_HPP