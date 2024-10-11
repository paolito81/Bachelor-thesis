#include <simulation.h>
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


Simulation::Simulation(const std::string& filename, const std::string& treename, const std::string& branchname) : 
	filename(filename), treename(treename), tree(nullptr), branchname(branchname), Edep(nullptr)
{
	loadTree();
};

Simulation::~Simulation() {
	if (tree) delete tree;
}

void Simulation::loadTree() {
	inFile = TFile::Open(filename.c_str(), "READ");
    
    if (!inFile || !inFile->IsOpen()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    tree = dynamic_cast<TTree*>(inFile->Get(treename.c_str()));
    if (!tree) {
        std::cerr << "Error: Could not find TTree " << treename << " in file " << filename << std::endl;
        return;
    }
}

void Simulation::analyzeTree() {
    if (!tree) {
        std::cerr << "Error: TTree not loaded" << std::endl;
        return;
    }
    
    int nEntries = tree->GetEntries();
    
    std::cout << "Analyzing TTree with " << nEntries << " entries." << std::endl;

    tree->SetBranchAddress(branchname.c_str(), Edep);
    // Loop through the entries
    for (int i = 0; i < nEntries; i++) {
        tree->GetEntry(i); // Load the entry

        // Now you can access the contents of the vector
        std::cout << "Entry " << i << ": ";
        for (size_t j = 0; j < Edep->size(); j++) {
            std::cout << (*Edep)[j] << " "; // Dereference the pointer to access vector elements
        }
        std::cout << std::endl;
    }

    // Cleanup
    //inFile->Close();
    //delete inFile;


}

void Simulation::scanTree() {
    //tree->Print();
    tree->Scan(branchname.c_str());
}


