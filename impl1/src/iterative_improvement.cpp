#include "iterative_improvement.hpp"
#include "init.hpp"
#include "pfspsolution.hpp"
#include <cstdlib>
#include <vector>
#include <utility> 
#include <iostream>
#include <experimental/optional>

std::vector<PfspSolution> transposeNeighbourhood(const PfspSolution& sol)
{
	std::vector<PfspSolution> res;
	for (size_t i = 0; i < sol.size() - 1; ++i) {
		PfspSolution neighbour(sol);
		std::swap(neighbour[i], neighbour[i+1]);
		res.push_back(neighbour);
	}
	return res;
}

std::vector<PfspSolution> exchangeNeighbourhood(const PfspSolution& sol)
{
	std::vector<PfspSolution> res;
	for (size_t i = 0; i < sol.size(); ++i) {
		for (size_t j = i + 1; j < sol.size(); ++j) {
			PfspSolution neighbour(sol);
			std::swap(neighbour[i], neighbour[j]);
			res.push_back(neighbour);
		}
	}
	return res;
}

std::vector<PfspSolution> insertNeighbourhood(const PfspSolution& sol)
{
	std::vector<PfspSolution> res;
	for (size_t i = 0; i < sol.size(); ++i) {
		for (size_t j = i + 1; j < sol.size(); ++j) {
			PfspSolution neighbour(sol);
			int element = neighbour[i];
			neighbour.erase(neighbour.begin() + i);
			neighbour.insert(neighbour.begin() + j - 1, element);
			res.push_back(neighbour);
		}
	}
	return res;
}

std::experimental::optional<PfspSolution> firstImprovement(const PfspInstance& instance, 
	                                                       const PfspSolution& sol, 
	                                                       const std::vector<PfspSolution>& neighbourhood) {
	double best_score = instance.computeScore(sol);
	for (auto neighbour : neighbourhood) {
		double score = instance.computeScore(neighbour);
		if (score < best_score) {
			return neighbour;
		}
	}
	return {};
}

std::experimental::optional<PfspSolution> bestImprovement(const PfspInstance& instance, 
	                                                       const PfspSolution& sol, 
	                                                       const std::vector<PfspSolution>& neighbourhood) {
	PfspSolution res(sol);
	double best_score = instance.computeScore(sol);
	for (auto neighbour : neighbourhood) {
		double score = instance.computeScore(neighbour);
		if (score < best_score) {
			best_score = score;
			res = neighbour;
		}
	}
	if (res == sol) {
		return {};
	}
	else {
		return res;
	}
}



IterativeImprovement::IterativeImprovement(const PfspInstance & instance, InitType init, ImprovementType impvmnt, std::vector<NeighbourhoodType> neighbourhoods)
{
	this->instance = instance;
	if (init == InitType::RANDOM) {
		initFunction = randomPermutation;
	}
	else if (init == InitType::SRZ) {
		initFunction = simplifiedRZ;
	}
	if (impvmnt == ImprovementType::FIRST) {
		improvementFunction = firstImprovement;
	}
	else if (impvmnt == ImprovementType::BEST) {
		improvementFunction = bestImprovement;
	}
	for (auto neighbourhood : neighbourhoods) {
		if (neighbourhood == NeighbourhoodType::TRANSPOSE) {
			neighbourhoodFunctions.push_back(transposeNeighbourhood);
		}
		else if (neighbourhood == NeighbourhoodType::EXCHANGE) {
			neighbourhoodFunctions.push_back(exchangeNeighbourhood);
		}
		else if (neighbourhood == NeighbourhoodType::INSERT) {
			neighbourhoodFunctions.push_back(insertNeighbourhood);
		}
	}
}

void IterativeImprovement::solve()
{
	initFunction(this->instance, this->solution);
	bool has_improved = false;
	std::cerr << "Initial :" << this->instance.computeScore(this->solution) << std::endl;
	do {
		for (auto neighbourhoodFunction : neighbourhoodFunctions) {
			auto neighbourhood = neighbourhoodFunction(this->solution);
			auto improvement = improvementFunction(this->instance, this->solution, neighbourhood);
			if (improvement) {
				has_improved = true;
				this->solution = improvement.value();
				std::cerr << "Improvement :" << this->instance.computeScore(this->solution) << std::endl;
				break;
			}
			else {
				has_improved = false;
			}
		}
	} while (has_improved);
}

PfspSolution IterativeImprovement::getSolution()
{
	return this->solution;
}