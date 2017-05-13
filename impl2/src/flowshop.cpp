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
#include <cstring>
#include "pfspinstance.hpp"
#include "pfspsolution.hpp"
#include "iterated_greedy.hpp"
#include "algorithm.hpp"



void display_usage(char* name)
{
    std::cout << "Usage: " << name << " <instance_file> <options>" << std::endl;
    std::cout << "where <options> are --ig --other (choose one)" << std::endl;
}

/***********************************************************************/


enum AlgoType {
    IG = 0,
    OTHER = 1
};

int main(int argc, char *argv[])
{
    if (argc < 2 or strcmp(argv[1], "-h") == 0 or strcmp(argv[1], "--help") == 0)
    {
        display_usage(argv[0]);
        return 0;
    }
    AlgoType type = AlgoType::IG;
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--ig") == 0) {
            type = AlgoType::IG;
        }
        else if (strcmp(argv[i], "--other") == 0) {
            type = AlgoType::OTHER;
        }
        else {
            std::cerr << "Argument not recognized " << argv[i] << std::endl;
        }
    }

    /* initialize random seed: */
    srand(42);

    /* Create instance object */
    PfspInstance instance;

    /* Read data from file */
    if (! instance.readDataFromFile(argv[1])) {
        return 1;
    }

    PfspSolution solution;
    Algorithm * algo;
    if (type == AlgoType::IG) {
        algo = new IteratedGreedy(instance);
    }
    else if (type == AlgoType::OTHER) {

    }
    algo->solve();
    solution = algo->getSolution();
    delete algo;

    std::cerr << "Solution: " ;
    printSolution(solution);

    /* Compute the TWT of this solution */
    long int totalWeightedTardiness = instance.computeScore(solution);
    std::cerr << "Total weighted completion time: ";
    std::cout << totalWeightedTardiness << std::endl;

    return 0;
}
