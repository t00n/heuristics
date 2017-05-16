#include "iterated_greedy.hpp"

#include <random>
#include <cmath>
#include "algorithm.hpp"

void IteratedGreedy::solve() {
	auto t = clock();
	this->solution = initLR(this->instance, this->instance.getNbJob() / this->instance.getNbMac());
	this->solution = iRZ(this->instance, this->solution);
	auto initial_solution = this->solution;
	auto best_solution = initial_solution;
	auto lambda = 2;
	auto d = 8;
	auto temperature = computeTemperature(lambda, this->instance);
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);
	do {
		auto solution1 = destruction_construction(this->instance, initial_solution, d);
		auto solution2 = iRZ(this->instance, solution1);
		if (instance.computeScore(solution2) < instance.computeScore(initial_solution)) {
			initial_solution = solution2;
			if (instance.computeScore(solution2) < instance.computeScore(best_solution)) {
				best_solution = solution2;
				std::cout << float(clock() - t) / CLOCKS_PER_SEC << ": " << instance.computeScore(best_solution) << std::endl;
			}
		}
		else if (dis(gen) < std::exp((instance.computeScore(initial_solution) - instance.computeScore(solution2)) / temperature)) {
			initial_solution = solution2;
		}
	}
	while (float(clock() - t) / CLOCKS_PER_SEC < this->timeout);
	this->solution = best_solution;
}