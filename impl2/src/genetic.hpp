#ifndef __GENETIC_HPP
#define __GENETIC_HPP

#include "algorithm.hpp"
#include "pfspsolution.hpp"

typedef std::vector<PfspSolution> Population;

class Genetic : public Algorithm {
public:
	using Algorithm::Algorithm;
	void solve();
};

#endif // __GENETIC_HPP