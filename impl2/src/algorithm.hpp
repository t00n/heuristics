#ifndef __ALGORITHM_HPP
#define __ALGORITHM_HPP

#include <random>
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

static std::random_device rd;

// IGA
PfspSolution randomSolution(const PfspInstance & instance);

void orderedSolution(const PfspInstance & instance, PfspSolution & sol);

void insert(PfspSolution & solution, size_t i, size_t j);

PfspSolution initLR(const PfspInstance &instance, unsigned int x);

PfspSolution RZ(const PfspInstance & instance, const PfspSolution& solution);

PfspSolution iRZ(const PfspInstance & instance, const PfspSolution& solution);

PfspSolution destruction_construction(const PfspInstance & instance, const PfspSolution & solution, unsigned int d);

int computeTemperature(int lambda, const PfspInstance & instance);

// Genetic
typedef std::vector<PfspSolution> Population;

Population randomPopulation(const PfspInstance & instance, size_t Ps);

std::vector<int> crossover_create_cut_points(const PfspInstance & instance);

void crossover_repair_child(PfspSolution & child, const PfspSolution & mother);

Population crossover_repair_children(const Population & children,
	                                 const PfspSolution & mother);

PfspSolution create_child_from_OA_line(const PfspSolution & mother,
	                                   const PfspSolution & father,
	                                   const std::vector<int> & OA_line,
	                                   const std::vector<int> & cut_points);

Population crossover_create_children(const std::vector<std::vector<int>> & OA,
	                                 const std::vector<int> & cut_points,
	                                 const PfspSolution & mother,
	                                 const PfspSolution & father);

std::vector<double> compute_several_scores(const PfspInstance & instance, const Population & pop);

std::vector<int> compute_best_factors(const PfspInstance & instance,
	                                  const std::vector<std::vector<int>> & OA,
	                                  const std::vector<double> scores,
	                                  size_t nbLevels);

PfspSolution OAcrossover(const PfspInstance & instance, const PfspSolution & mother, const PfspSolution & father);
#endif // __ALGORITHM_HPP