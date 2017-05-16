#include "genetic.hpp"

#include <algorithm>
#include <limits>
#include <random>
#include <ctime>
#include <set>
#include <cassert>
#include "ortho_arrays.hpp"

Population initRandom(const PfspInstance & instance, size_t Ps) {
	Population pop;
	for (size_t i = 0; i < Ps; ++i) {
		pop.push_back(randomSolution(instance));
	}
	return pop;
}

std::vector<int> crossover_create_cut_points(const PfspInstance & instance) {
	auto OAsize = instance.getNbJob() == 50 ? 15 : 30;
	std::vector<int> cut_points;
	static std::mt19937 gen(42);
	std::uniform_int_distribution<> dis(0, instance.getNbJob() - 1);
	for (size_t i = 0; i < OAsize; ++i) {
		int cut;
		do {
			cut = dis(gen);
		} while (std::find(cut_points.begin(), cut_points.end(), cut) != cut_points.end());
		cut_points.push_back(cut);
	}
	std::sort(cut_points.begin(), cut_points.end());
	cut_points.push_back(instance.getNbJob());
	return cut_points;
}

void crossover_repair_child(PfspSolution & child, const PfspSolution & mother) {
	if (std::set<int>(child.begin(), child.end()).size() < child.size())  { // there are duplicates
		std::vector<int> not_in_child;
		std::set_difference(mother.begin(), mother.end(),
			                child.begin(), child.end(),
			                std::inserter(not_in_child, not_in_child.begin()));
		auto i = 0;
		for (auto it = child.begin(); it != child.end(); ++it) {
			if (std::find(child.begin(), it, *it) != it) {
				while (std::find(child.begin(), it, not_in_child[i]) != it) {
					++i;
				}
				*it = not_in_child[i];
			}
		}
		assert(std::set<int>(child.begin(), child.end()).size() == 100); // assert that duplicates are gone
	}
}

Population crossover_repair_children(const Population & children,
	                                 const PfspSolution & mother) {
	Population res;
	for (auto child : children) {
		crossover_repair_child(child, mother);
		res.push_back(child);
	}
	return res;
}

PfspSolution create_child_from_OA_line(const PfspSolution & mother,
	                                   const PfspSolution & father,
	                                   const std::vector<int> & OA_line,
	                                   const std::vector<int> & cut_points) {
	PfspSolution child;
	auto curr_cut_point = 0;
	auto curr_OA = 0;
	size_t i = 0;
	while (i < mother.size()) {
		while (i <= cut_points[curr_cut_point] and i < mother.size()) {
			if (OA_line[curr_OA] == 0) {
				child.push_back(mother[i]);
			}
			else if (OA_line[curr_OA] == 1) {
				child.push_back(father[i]);
			}
			++i;
		}
		curr_cut_point++;
		curr_OA++;
	}
	return child;
}

Population crossover_create_children(const std::vector<std::vector<int>> & OA,
	                                 const std::vector<int> & cut_points,
	                                 const PfspSolution & mother,
	                                 const PfspSolution & father) {
	Population children;
	for (auto OA_line : OA) {
		auto child = create_child_from_OA_line(mother, father, OA_line, cut_points);
		children.push_back(child);
	}
	return children;
}

std::vector<double> compute_several_scores(const PfspInstance & instance, const Population & pop) {
	std::vector<double> res;
	for (auto sol : pop) {
		res.push_back(instance.computeScore(sol));
	}
	return res;
}

std::vector<int> compute_best_factors(const PfspInstance & instance,
	                                  const std::vector<std::vector<int>> & OA,
	                                  const std::vector<double> scores,
	                                  size_t nbLevels) {
	std::vector<int> res;
	for (size_t j = 0; j < OA[0].size(); ++j) {
		std::vector<double> line;
		for (size_t k = 0; k < nbLevels; ++k) {
			auto val = 0;
			for (size_t i = 0; i < OA.size(); ++i) {
				val += scores[i] * (OA[i][j] - 1 == k);
			}
			line.push_back(val);
		}
		res.push_back(std::distance(line.begin(), std::max_element(line.begin(), line.end())));
	}
	return res;
}

PfspSolution OAcrossover(const PfspInstance & instance, const PfspSolution & mother, const PfspSolution & father) {
	// init orthogonal arrays
	auto OA = instance.getNbJob() == 50 ? fifteen_by_two : thirty_by_two;
	auto cut_points = crossover_create_cut_points(instance);
	// get N - 1 cut_points
	Population children = crossover_create_children(OA, cut_points, mother, father);
	children = crossover_repair_children(children, mother);
	auto children_scores = compute_several_scores(instance, children);
	auto best_factors = compute_best_factors(instance, OA, children_scores, 2);
	auto best_child = create_child_from_OA_line(mother, father, best_factors, cut_points);
	crossover_repair_child(best_child, mother);
	children.push_back(best_child);
	auto best_score = std::numeric_limits<double>::infinity();
	for (auto child : children) {
		auto score = instance.computeScore(child);
		if (score < best_score) {
			best_score = score;
			best_child = child;
		}
	}
	return best_child;
}

Population OAmutation(Population pop) {
	return pop;
}

Population selection(Population pop) {
	return pop;
}

void Genetic::solve() {
	// step 1 : init vars
	std::mt19937 gen(42);
	auto t = clock();
	size_t Ps = 100;
	float Pm = 1.05;
	int Max_Stuck = 10;
	int TLS = 5;
	int Stuck_loop = 0;
	// step 2 : init pop randomly
	Population pop = initRandom(this->instance, Ps);
	// step 3 : find best solution
	PfspSolution best_solution;
	double best_score = std::numeric_limits<double>::infinity();
	for (auto sol : pop) {
		double score = this->instance.computeScore(sol);
		if (score < best_score) {
			best_score = score;
			best_solution = sol;
		}
	}
	do {
		// Step 4 : apply Step 5-7 Ps times
		for (size_t i = 0; i < Ps; ++i) {
			std::uniform_int_distribution<> dis(0, Ps - 1);
			auto mother = dis(gen);
			auto father = mother;
			do {
				father = dis(gen);
			} while (father == mother);
			auto child = OAcrossover(this->instance, pop[mother], pop[father]);
			auto mother_score = instance.computeScore(pop[mother]);
			auto father_score = instance.computeScore(pop[father]);
			auto child_score = instance.computeScore(child);
			if ((child_score < mother_score or child_score < father_score) and Stuck_loop > TLS) {
				child = RZ(instance, child);
				child_score = instance.computeScore(child);
			}
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
	} while (float(clock() - t) / CLOCKS_PER_SEC < this->timeout);
	this->solution = best_solution;
}