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
#include "init.hpp"
#include "pfspsolution.hpp"
#include "iterative_improvement.hpp"



void display_usage(char* name)
{
    std::cout << "Usage: " << name << " <instance_file> <options>" << std::endl;
    std::cout << "where <options> are --first --best (choose one)" << std::endl;
    std::cout << "                    --transpose --exchange --insert --tie --tei (choose one)" << std::endl;
    std::cout << "                    --random-init --srz (choose one)" << std::endl;
    std::cout << "                    -h --help (this help)" << std::endl;
}

/***********************************************************************/

int main(int argc, char *argv[])
{
    long int totalWeightedTardiness;


    if (argc < 2 or strcmp(argv[1], "-h") == 0 or strcmp(argv[1], "--help") == 0)
    {
        display_usage(argv[0]);
        return 0;
    }
    ImprovementType improvement = ImprovementType::FIRST;
    std::vector<NeighbourhoodType> neighbourhoods = {};
    InitType init = InitType::RANDOM;
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--first") == 0) {
            improvement = ImprovementType::FIRST;
        }
        else if (strcmp(argv[i], "--best") == 0) {
            improvement = ImprovementType::BEST;
        }
        else if (strcmp(argv[i], "--transpose") == 0) {
            neighbourhoods = {NeighbourhoodType::TRANSPOSE};
        }
        else if (strcmp(argv[i], "--exchange") == 0) {
            neighbourhoods = {NeighbourhoodType::EXCHANGE};
        }
        else if (strcmp(argv[i], "--insert") == 0) {
            neighbourhoods = {NeighbourhoodType::INSERT};
        }
        else if (strcmp(argv[i], "--random-init") == 0) {
            init = InitType::RANDOM;
        }
        else if (strcmp(argv[i], "--srz") == 0) {
            init = InitType::SRZ;
        }
        else if (strcmp(argv[i], "--tei") == 0) {
            init = InitType::SRZ;
            improvement = ImprovementType::FIRST;
            neighbourhoods = {NeighbourhoodType::TRANSPOSE, NeighbourhoodType::EXCHANGE, NeighbourhoodType::INSERT};
        }
        else if (strcmp(argv[i], "--tie") == 0) {
            init = InitType::SRZ;
            improvement = ImprovementType::FIRST;
            neighbourhoods = {NeighbourhoodType::TRANSPOSE, NeighbourhoodType::INSERT, NeighbourhoodType::EXCHANGE};
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

    IterativeImprovement algo(instance, init, improvement, neighbourhoods);
    algo.solve();
    PfspSolution solution = algo.getSolution();

    std::cerr << "Solution: " ;
    printSolution(solution);

    /* Compute the TWT of this solution */
    totalWeightedTardiness = instance.computeScore(solution);
    std::cerr << "Total weighted completion time: ";
    std::cout << totalWeightedTardiness << std::endl;

    return 0;
}
