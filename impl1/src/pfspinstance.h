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


#ifndef _PFSPINSTANCEWT_H_
#define _PFSPINSTANCEWT_H_

#include <string>
#include <vector>

using namespace std;

class PfspInstance{
  private:
    int nbJob;
    int nbMac;
    std::vector< long int > dueDates;
    std::vector< long int > priority;

    std::vector<std::vector<long int>> processingTimesMatrix;

    /* Allow the memory for the processing times matrix : */
    void allowMatrixMemory(int nbJ, int nbM);

    // void setDueDate(int job, int value);
    // void setTime(int job, int machine, long int processTime);
    // void setPriority(int job, int value);

  public:
    PfspInstance();
    ~PfspInstance();

    /* Read write privates attributs : */
    int getNbJob();
    int getNbMac();

    /* Read\Write values in the matrix : */
    long int getTime(int job, int machine);

    long int getDueDate(int job);

    long int getPriority(int job);

    /* Read Data from a file : */
    bool readDataFromFile(char * fileName);

    long int computeScore(vector<int>& sol);
};

#endif
