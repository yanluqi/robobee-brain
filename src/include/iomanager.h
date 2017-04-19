/*
 *  iomanager.h
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

#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class Iomanager
{
public:
	// Constructor
	Iomanager(const std::string& _loadDir, const std::string& _saveDir);

	// Default Constructor
	Iomanager();

	// Destroyer
	~Iomanager();

	void SetStream(const std::string& fname, const std::string& stype);

	std::ifstream& Read();
	std::ofstream& Print();

	// Save Data in a file
	void SaveVec(const std::string& fname, std::vector<double> *vect);

	// Load Data from a file
	std::vector<double> LoadVec(const std::string& fname);

protected:

private:
	std::string loadDir;
	std::string saveDir;

	std::ofstream outStream;
	std::ifstream inStream;
};

#endif
