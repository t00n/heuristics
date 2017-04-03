#ifndef __ITERATIVE_IMPROVEMENT_HPP
#define __ITERATIVE_IMPROVEMENT_HPP

#include <functional>
#include <vector>
#include <experimental/optional>

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
	PfspInstance instance;
	PfspSolution solution;
	std::function<void(const PfspInstance & instance, PfspSolution&)> initFunction;
	std::function<std::experimental::optional<PfspSolution>(const PfspInstance&, const PfspSolution&, const std::vector<PfspSolution>&)> improvementFunction;
	std::vector<std::function<std::vector<PfspSolution>(const PfspSolution&)>> neighbourhoodFunctions;

public:
	IterativeImprovement(const PfspInstance & instance, InitType init, ImprovementType impvmnt, std::vector<NeighbourhoodType> neighbourhoods);

	void solve();

	PfspSolution getSolution();
};

#endif // __ITERATIVE_IMPROVEMENT_HPP