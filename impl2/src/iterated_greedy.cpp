#include "iterated_greedy.hpp"

#include <algorithm>
#include <limits>

void orderedSolution(const PfspInstance & instance, PfspSolution & sol)
{
	for (int i = 0; i < instance.getNbJob(); ++i) {
		sol.push_back(i);
	}
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
		unsigned int job = ordered_by_process_time[i];
		ordered_by_process_time.erase(ordered_by_process_time.begin() + i);
		ordered_by_process_time.insert(ordered_by_process_time.begin(), job);
		unsigned int score = instance.computeScore(ordered_by_process_time);
		if (score < best_score) {
			best_score = score;
			best_solution = ordered_by_process_time;
		}
		ordered_by_process_time.erase(ordered_by_process_time.begin());
		ordered_by_process_time.insert(ordered_by_process_time.begin() + i, job);
	}
	solution = best_solution;
}

void IteratedGreedy::solve() {
	initLR(this->instance, this->solution, this->instance.getNbJob() / this->instance.getNbMac());
}