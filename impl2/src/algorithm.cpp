#include "algorithm.hpp"

#include <algorithm>
#include <random>
#include <limits>

Algorithm::Algorithm(const PfspInstance & instance, std::time_t timeout) {
	this->instance = instance;
	this->timeout = timeout;
}

PfspSolution Algorithm::getSolution() {
	return this->solution;
}

void orderedSolution(const PfspInstance & instance, PfspSolution & sol)
{
	for (int i = 0; i < instance.getNbJob(); ++i) {
		sol.push_back(i);
	}
}

void insert(PfspSolution & solution, size_t i, size_t j) {
	unsigned int job = solution[i];
	solution.erase(solution.begin() + i);
	solution.insert(solution.begin() + j, job);
}

PfspSolution initLR(const PfspInstance &instance, unsigned int x) {
	PfspSolution ordered_by_process_time;
	orderedSolution(instance, ordered_by_process_time);
	std::stable_sort(
		ordered_by_process_time.begin(),
		ordered_by_process_time.end(),
		[&instance](int a, int b) {
		      double weightA = 0, weightB = 0;
		      for (int m = 0; m < instance.getNbMac(); ++m) {
		          weightA += instance.getTime(a, m);
		          weightB += instance.getTime(b, m);
		      }
		      weightA /= instance.getPriority(a);
		      weightB /= instance.getPriority(b);
		      return weightA < weightB;
		});
	double best_score = std::numeric_limits<double>::infinity();
	PfspSolution best_solution;
	for (size_t i = 0; i < x; ++i) {
		insert(ordered_by_process_time, i, 0);
		unsigned int score = instance.computeScore(ordered_by_process_time);
		if (score < best_score) {
			best_score = score;
			best_solution = ordered_by_process_time;
		}
		insert(ordered_by_process_time, 0, i);
	}
	return best_solution;
}

PfspSolution RZ(const PfspInstance & instance, const PfspSolution& solution) {
	double best_score = std::numeric_limits<double>::infinity();
	PfspSolution best_solution;
	auto sol = solution;
	for (size_t i = 0; i < sol.size(); ++i) {
		for (size_t j = 0; j < sol.size(); ++j) {
			insert(sol, i, j);
			double score = instance.computeScore(sol);
			if (score < best_score) {
				best_solution = sol;
				best_score = score;
			}
			insert(sol, j, i);
		}
	}
	return best_solution;
}

PfspSolution iRZ(const PfspInstance & instance, const PfspSolution& solution) {
	PfspSolution old_solution;
	PfspSolution new_solution = solution;
	do {
		old_solution = new_solution;
		new_solution = RZ(instance, old_solution);
	} while (new_solution != old_solution);
	return new_solution;
}

PfspSolution destruction_construction(const PfspInstance & instance, const PfspSolution & solution, unsigned int d) {
	// destruction
	static std::mt19937 gen(42);
	PfspSolution jobs;
	PfspSolution new_solution = solution;
	for (unsigned int i = 0; i < d; ++i) {
    	std::uniform_int_distribution<> dis(0, new_solution.size() - 1);
    	auto r = dis(gen);
    	auto job = new_solution[r];
    	new_solution.erase(new_solution.begin() + r);
    	jobs.push_back(job);
	}
	// construction
	for (auto job : jobs) {
		PfspSolution best_solution;
		auto best_score = std::numeric_limits<double>::infinity();
		for (auto j = 0; j < new_solution.size(); ++j) {
			 new_solution.insert(new_solution.begin() + j, job);
			 auto score = instance.computeScore(new_solution);
			 if (score < best_score) {
			 	best_score = score;
			 	best_solution = new_solution;
			 }
			 new_solution.erase(new_solution.begin() + j);
		}
		new_solution = best_solution;
	}
	return new_solution;
}

int computeTemperature(int lambda, const PfspInstance & instance) {
	int res = 0;
	for (size_t i = 0; i < instance.getNbJob(); ++i) {
		for (size_t j = 0; j < instance.getNbMac(); ++j) {
			res += instance.getTime(i, j);
		}
	}
	return (lambda * res) / (10 * instance.getNbJob() * instance.getNbMac());
}