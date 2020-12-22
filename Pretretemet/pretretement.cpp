#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <stdlib.h> 
#include <cstdlib>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;


int main() {


	string line;
	ifstream datafile("parseDaataSet.txt");
	ofstream monFlux("training.txt");
	ofstream otherFlux("testing.txt");
	std::ofstream parsedata;
	//parsedata.open("parseDaataSet.txt", std::ios_base::app);
	monFlux << "static const std::vector<string> database{" << "\n";
	cout << "static const std::vector<string> database{" << "\n";

	if (datafile.is_open())
	{
		for (size_t i = 0; i < 10000; i++)
		{
			getline(datafile, line);
			line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
			monFlux << "\"" <<line << "\",\n";
		}
		for (size_t i = 0; i < 5000; i++)
		{
			getline(datafile, line);
			line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
			otherFlux << line << "\n";
		}

	}
	monFlux << "};" << "\n";
	datafile.close();
	monFlux.close();
	otherFlux.close();

}