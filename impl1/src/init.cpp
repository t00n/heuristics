#include "init.hpp"

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include "pfspsolution.hpp"

void orderedSolution(const PfspInstance & instance, PfspSolution & sol)
{
	for (int i = 0; i < instance.getNbJob(); ++i) {
		sol.push_back(i);
	}
}

/* Fill the solution with numbers between 1 and nbJobs, shuffled */
void randomPermutation(const PfspInstance & instance, PfspSolution & sol)
{
	orderedSolution(instance, sol);
    std::random_shuffle(sol.begin(), sol.end());
}

void simplifiedRZ(const PfspInstance & instance, PfspSolution & sol)
{
	PfspSolution ordered_by_process_time;
	orderedSolution(instance, ordered_by_process_time);
	// sort the jobs by ascending order of weighted processing time
	std::stable_sort(ordered_by_process_time.begin(),
		      ordered_by_process_time.end(),
		      [&instance](int a, int b) {
	      	      double weightA = 0, weightB = 0;
	      	      for (int m = 0; m < instance.getNbMac(); ++m) {
	      	          weightA += instance.getTime(a, m);
	      	          weightB += instance.getTime(b, m);
	      	      }
	      	      weightA /= instance.getPriority(a);
	      	      weightB /= instance.getPriority(b);
	      	      return weightA <= weightB;
		      });

	// add each job in the position that minimizes the score of the partial solution
	for (auto job : ordered_by_process_time)
	{
		size_t best_i = 0;
		double best_score = std::numeric_limits<double>::infinity();
		for (size_t i = 0; i <= sol.size(); ++i)
		{
			sol.insert(sol.begin() + i, job);
			double score = instance.computeScore(sol);
			if (score < best_score) {
				best_score = score;
				best_i = i;
			}
			sol.erase(sol.begin() + i);
		}
		sol.insert(sol.begin() + best_i, job);
	}
}