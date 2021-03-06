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
#include <fstream>
#include <cstring>
#include "pfspinstance.hpp"

PfspInstance::PfspInstance()
{
}


PfspInstance::~PfspInstance()
{
}

int PfspInstance::getNbJob() const
{
    return nbJob;
}

int PfspInstance::getNbMac() const
{
    return nbMac;
}



/* Allow the memory for the processing times matrix : */
void PfspInstance::allowMatrixMemory(int nbJ, int nbM)
{
    processingTimesMatrix.resize(nbJ);          // 1st dimension

    for (int cpt = 0; cpt < nbJ; ++cpt) {
        processingTimesMatrix[cpt].resize(nbM); // 2nd dimension
    }

    dueDates.resize(nbJ);
    priority.resize(nbJ);
}


long int PfspInstance::getTime(int job, int machine) const
{
    if ((job < 0) || (job >= nbJob) || (machine < 0) || (machine >= nbMac)) {
      std::cerr    << "ERROR. file:pfspInstance.cpp, method:getTime. Out of bound. job=" << job
          << ", machine=" << machine << std::endl;
    }

    return processingTimesMatrix[job][machine];
}


long int PfspInstance::getDueDate(int job) const
{
    if ((job < 0) || (job >= nbJob)) {
      std::cerr    << "ERROR. file:pfspInstance.cpp, method:getDueDate. Out of bound. job=" << job << std::endl;
    }

    return dueDates[job];
}

long int PfspInstance::getPriority(int job) const
{
    if ((job < 0) || (job >= nbJob)) {
      std::cerr    << "ERROR. file:pfspInstance.cpp, method:getPriority. Out of bound. job=" << job << std::endl;
    }

    return priority[job];
}

/* Read the instance from file : */
bool PfspInstance::readDataFromFile(char * fileName)
{
	bool everythingOK = true;
	int j, m; // iterators
	long int readValue;
	std::string str;
	std::ifstream fileIn;
	char * aux2;
	char fileNameOK[100] = "";

	aux2 = (strrchr(fileName, '/'));

	if (aux2 == NULL)
		aux2 = fileName;
	else
		aux2 += 1;

	strcat(fileNameOK, aux2);

	std::cerr << "name : " << fileNameOK << std::endl;
	std::cerr << "file : " << fileName << std::endl;

	fileIn.open(fileName);

	if (fileIn.is_open()) {
        std::cerr << "File " << fileName << " is now open, start to read..." << std::endl;

		fileIn >> nbJob;
        std::cerr << "Number of jobs : " << nbJob << std::endl;
		fileIn >> nbMac;
        std::cerr << "Number of machines : " << nbMac << std::endl;
        std::cerr << "Allow memory for the matrix..." << std::endl;
		allowMatrixMemory(nbJob, nbMac);
        std::cerr << "Memory allowed." << std::endl;
        std::cerr << "Start to read matrix..." << std::endl;

		for (j = 0; j < nbJob; ++j)
		{
			for (m = 0; m < nbMac; ++m)
			{
				fileIn >> readValue; // The number of each machine, not important !
				fileIn >> readValue; // Process Time

				processingTimesMatrix[j][m] = readValue;
			}
		}
        fileIn >> str; // this is not read

		for (j = 0; j < nbJob; ++j)
		{
			fileIn >> readValue; // -1
			fileIn >> readValue;
			dueDates[j] = readValue;
			fileIn >> readValue; // -1
			fileIn >> readValue;
            priority[j] = readValue;
		}

        std::cerr << "All is read from file." << std::endl;
		fileIn.close();
	}
	else
	{
		std::cerr    << "ERROR. file:pfspInstance.cpp, method:readDataFromFile, "
				<< "error while opening file " << fileName << std::endl;

		everythingOK = false;
	}

	return everythingOK;
}


/* Compute the weighted tardiness of a given solution */
long int PfspInstance::computeScore(const PfspSolution& sol) const
{
	size_t j;
    int m;
	int jobNumber;
	long int score;

	/* We need end times on previous machine : */
	std::vector<long int> previousMachineEndTime(sol.size());
	/* And the end time of the previous job, on the same machine : */

	/* 1st machine : */
	previousMachineEndTime[0] = processingTimesMatrix[sol[0]][0];
	for (j = 1; j < sol.size(); ++j)
	{
		jobNumber = sol[j];
		previousMachineEndTime[j] = previousMachineEndTime[j-1] + processingTimesMatrix[jobNumber][0];
	}

	/* others machines : */
	for (m = 1; m < nbMac; ++m)
	{
		for (j = 0; j < sol.size(); ++j)
		{
			jobNumber = sol[j];

            previousMachineEndTime[j] = std::max(
                previousMachineEndTime[j],
                (j == 0 ? 0 : previousMachineEndTime[j-1])
           ) + processingTimesMatrix[jobNumber][m];
		}
	}

	score = 0;
	for (j = 0; j < sol.size(); ++j)
	    score += previousMachineEndTime[j] * priority[sol[j]];

	return score;
}
