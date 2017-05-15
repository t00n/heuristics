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

#endif // __ALGORITHM_HPP