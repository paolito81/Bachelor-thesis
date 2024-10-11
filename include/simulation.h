#ifndef SIMULATION_H
#define SIMULATION_H

#include <TTree.h>
#include <analyzer.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include "utils.h"
#include <fstream>
#include <cmath>
#include <TTree.h>
#include <TStyle.h>
#include <TMath.h>
#include <TLine.h>
#include <TLegend.h>
#include <vector>


class Simulation {
public:
	Simulation(const std::string& filename, const std::string& treename, const std::string& branchname);
	~Simulation();
	void loadTree();
	void analyzeTree();
	void scanTree();
private:
	TFile* inFile;
	TTree* tree;
	std::string filename;
	std::string treename;
	std::string branchname;
	std::vector<Double_t>* Edep;
};

#endif // SIMULATION_H
