#include <string>
#include <TFile.h>
#include <TTree.h>
#include <iostream>

void readfile(int filenumber) {
	if (filenumber < 1771 || filenumber > 1881)
	{
		std::cerr << "Invalid file number!";
		exit(1);
	}

	std::string filename = "root" + std::to_string(filenumber) + ".root";
	/*
	TFile *file = TFile::Open(filename);

	if (!file || file->IsZombie())
	{
		std::cerr << "Failed to open file: " << filename << std::endl;
		exit(1);
	}*/

}