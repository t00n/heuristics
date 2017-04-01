#include "init.hpp"

#include <vector>
#include <cstdlib>
#include <algorithm>

/* Fill the solution with numbers between 1 and nbJobs, shuffled */
void randomPermutation(std::vector<int> & sol)
{
    for (size_t i = 0; i < sol.size(); ++i) {
        sol[i] = i;
    }
    std::random_shuffle(sol.begin(), sol.end());
}