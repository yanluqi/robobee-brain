/*
 *  iomanager.cpp
 *
 *  This file is part of RoboBrain.
 *  Copyright (C) 2016 Bernardo Fichera
 *
 *  RoboBrain is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RoboBrain is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RoboBrain.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "include/iomanager.h"

Iomanager::Iomanager(std::string& _loadDir, std::string& _saveDir)
{
        loadDir = _loadDir + "/";
        saveDir = _saveDir + "/";
}

Iomanager::Iomanager()
{
	
}

Iomanager::~Iomanager()
{

}

void Iomanager::PrintData(std::string fname, std::vector <double> *vect){
    std::string path = saveDir + fname;
    std::ofstream outData (path.c_str(), std::ofstream::out);

    for (int i=0; i != vect->size(); ++i)
        outData << vect->at(i) << std::endl;

    outData.close();
}// end printData

std::vector <double> Iomanager::ReadData(std::string fname) {
    std::string path = loadDir + fname;
    std::ifstream inData (path.c_str());
    std::vector <double> output;
    double insert;

    while(inData){
        inData >> insert;
        output.push_back(insert);
    }

    inData.close();
    output.pop_back();

    return output;
} // end getData