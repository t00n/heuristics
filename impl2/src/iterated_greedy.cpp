#include "iterated_greedy.hpp"

#include <algorithm>
#include <limits>

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

void initLR(const PfspInstance &instance, PfspSolution& solution, unsigned int x) {
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
	solution = best_solution;
}

void RZ(const PfspInstance & instance, PfspSolution& solution) {
	double best_score = std::numeric_limits<double>::infinity();
	PfspSolution best_solution;
	for (size_t i = 0; i < solution.size(); ++i) {
		for (size_t j = 0; j < solution.size(); ++j) {
			insert(solution, i, j);
			double score = instance.computeScore(solution);
			if (score < best_score) {
				best_solution = solution;
				best_score = score;
			}
			insert(solution, j, i);
		}
	}
	solution = best_solution;
}

void iRZ(const PfspInstance & instance, PfspSolution& solution) {
	PfspSolution old_solution;
	do {
		old_solution = solution;
		RZ(instance, solution);
	} while (old_solution != solution);
}

void IteratedGreedy::solve() {
	initLR(this->instance, this->solution, this->instance.getNbJob() / this->instance.getNbMac());
	iRZ(this->instance, this->solution);
}