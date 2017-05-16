#include "genetic.hpp"

#include <algorithm>
#include <limits>
#include <ctime>

void Genetic::solve() {
	// step 1 : init vars
	std::mt19937 gen(rd());
	auto t = clock();
	size_t Ps = 20;
	float Pm = 0.8;
	int TLS = 5;
	int Stuck_loop = 0;
	// step 2 : init pop randomly
	Population pop = randomPopulation(this->instance, Ps);
	// step 3 : find best solution
	PfspSolution best_solution;
	double best_score = std::numeric_limits<double>::infinity();
	std::uniform_int_distribution<> dis(0, Ps - 1);
	do {
		// Step 4 : apply Step 5-7 Ps times
		for (size_t i = 0; i < Ps; ++i) {
			auto mother = dis(gen);
			auto father = mother;
			do {
				father = dis(gen);
			} while (father == mother);
			// Step 5 : crossover to create child
			auto child = OAcrossover(this->instance, pop[mother], pop[father]);
			auto mother_score = instance.computeScore(pop[mother]);
			auto father_score = instance.computeScore(pop[father]);
			auto child_score = instance.computeScore(child);
			// Step 6 : use local search if stuck for too long
			if ((child_score < mother_score or child_score < father_score) and Stuck_loop > TLS) {
				child = iRZ(instance, child);
				child_score = instance.computeScore(child);
			}
			// Step 7 : replace worst parent by child
			if (child_score < mother_score and mother_score < father_score) {
				pop[father] = child;
			}
			else if (child_score < father_score and father_score < mother_score) {
				pop[mother] = child;
			}
		}
		// Step 8 : find new best
		auto improved = false;
		for (auto sol : pop) {
			auto score = instance.computeScore(sol);
			if (score < best_score) {
				best_solution = sol;
				best_score = score;
				improved = true;
			}
		}
		if (improved) {
			Stuck_loop = 0;
			std::cout << float(clock() - t) / CLOCKS_PER_SEC << ": " << best_score << std::endl;
		}
		else {
			Stuck_loop++;
		}
		// Step 9 ; mutation
		for (size_t i = 0; i < Ps * Pm; ++i) {
			auto r = dis(gen);
			pop[r] = destruction_construction(instance, pop[r], 8);
		}
	} while (float(clock() - t) / CLOCKS_PER_SEC < this->timeout);
	this->solution = best_solution;
}