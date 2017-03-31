/***************************************************************************
 *   Copyright (C) 2012 by Jérémie Dubois-Lacoste   *
 *   jeremie.dl@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "pfspinstance.h"

using namespace std;


/* Fill the solution with numbers between 1 and nbJobs, shuffled */
void randomPermutation(vector< int > & sol)
{
    for (size_t i = 0; i < sol.size(); ++i) {
        sol[i] = i;
    }
    std::random_shuffle(sol.begin(), sol.end());
}

/***********************************************************************/

int main(int argc, char *argv[])
{
  int i;
  long int totalWeightedTardiness;


  if (argc == 1)
  {
    cout << "Usage: " << argv[0] << " <instance_file>" << endl;
    return 0;
  }

  /* initialize random seed: */
  srand(42);

  /* Create instance object */
  PfspInstance instance;

  /* Read data from file */
  if (! instance.readDataFromFile(argv[1]) )
    return 1;

  /* Create a vector of int to represent the solution
    WARNING: By convention, we store the jobs starting from index 1,
             thus the size nbJob + 1. */
  vector<int> solution(instance.getNbJob());

  /* Fill the vector with a random permutation */
  randomPermutation(solution);

  cout << "Random solution: " ;
  for (i = 0; i < instance.getNbJob(); ++i)
    cout << solution[i] << " " ;
  cout << endl;

  /* Compute the TWT of this solution */
  totalWeightedTardiness = instance.computeScore(solution);
  cout << "Total weighted completion time: " << totalWeightedTardiness << endl;

  return 0;
}
