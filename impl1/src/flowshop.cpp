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
#include "pfspinstance.hpp"
#include "init.hpp"


template<typename T>
void printVector(std::vector<T> const & v) {
    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

typedef std::vector<int> PfspSolution;

void printSolution(PfspSolution const & sol) {
    PfspSolution v(sol);
    for (auto it = v.begin(); it != v.end(); ++it) {
        (*it)++;
    }
    printVector(v);
}


/***********************************************************************/

int main(int argc, char *argv[])
{
    long int totalWeightedTardiness;


    if (argc == 1)
    {
        std::cout << "Usage: " << argv[0] << " <instance_file>" << std::endl;
        return 0;
    }

    /* initialize random seed: */
    srand(42);

    /* Create instance object */
    PfspInstance instance;

    /* Read data from file */
    if (! instance.readDataFromFile(argv[1])) {
        return 1;
    }

    PfspSolution solution(instance.getNbJob());

    /* Fill the vector with a random permutation */
    randomPermutation(solution);

    std::cout << "Random solution: " ;
    printSolution(solution);

    /* Compute the TWT of this solution */
    totalWeightedTardiness = instance.computeScore(solution);
    std::cout << "Total weighted completion time: " << totalWeightedTardiness << std::endl;

    return 0;
}
