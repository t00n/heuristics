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



/***********************************************************************/

int main(int argc, char *argv[])
{
    long int totalWeightedTardiness;


    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <instance_file> <options>" << std::endl;
        std::cout << "where <options> are --first --best" << std::endl;
        std::cout << "                    --transpose --exchange --insert" << std::endl;
        std::cout << "                    --random-init --srz" << std::endl;
        return 0;
    }
    int first = 0;
    int transpose = 0;
    int random = 0;
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--first") == 0) {
            first = 0;
        }
        else if (strcmp(argv[i], "--best") == 0) {
            first = 1;
        }
        else if (strcmp(argv[i], "--transpose") == 0) {
            transpose = 0;
        }
        else if (strcmp(argv[i], "--exchange") == 0) {
            transpose = 1;
        }
        else if (strcmp(argv[i], "--insert") == 0) {
            transpose = 2;
        }
        else if (strcmp(argv[i], "--random-init") == 0) {
            random = 0;
        }
        else if (strcmp(argv[i], "--srz") == 0) {
            random = 1;
        }
        else {
            std::cout << "Argument not recognized " << argv[i] << std::endl;
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

    if (random == 0) {
        /* Fill the vector with a random permutation */
        randomPermutation(instance, solution);
    }
    else {
        simplifiedRZ(instance, solution);
    }

    if (first == 0) {

    }
    else {

    }

    if (transpose == 0) {

    }
    else if (transpose == 1) {

    }
    else {

    }

    std::cout << "Solution: " ;
    printSolution(solution);

    /* Compute the TWT of this solution */
    totalWeightedTardiness = instance.computeScore(solution);
    std::cout << "Total weighted completion time: " << totalWeightedTardiness << std::endl;

    return 0;
}
