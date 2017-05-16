#include "algorithm.hpp"

#include <algorithm>
#include <random>
#include <limits>
#include <set>
#include <cassert>
#include "ortho_arrays.hpp"

Algorithm::Algorithm(const PfspInstance & instance, std::time_t timeout) {
	this->instance = instance;
	this->timeout = timeout;
}

PfspSolution Algorithm::getSolution() {
	return this->solution;
}

// IGA
PfspSolution randomSolution(const PfspInstance & instance) {
	PfspSolution solution;
	orderedSolution(instance, solution);
	std::random_shuffle(solution.begin(), solution.end());
	return solution;
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
	static std::mt19937 gen(rd());
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

// Genetic

Population randomPopulation(const PfspInstance & instance, size_t Ps) {
	Population pop;
	for (size_t i = 0; i < Ps; ++i) {
		pop.push_back(randomSolution(instance));
	}
	return pop;
}

std::vector<int> crossover_create_cut_points(const PfspInstance & instance) {
	auto OAsize = instance.getNbJob() == 50 ? 14 : 30;
	std::vector<int> cut_points;
	static std::mt19937 gen(rd());
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
				val += scores[i] * (OA[i][j] == k);
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