#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include "run1776.h"

void run1776 ()
{
	TCanvas* c1 = new TCanvas();

	TFile* inFile = new TFile("../../../root files/run1776_coinc.root", "read");

	if (!inFile || inFile->IsZombie())
	{
		std::cerr << "Failed to open file: " << "run1776_coinc.root" << std::endl;
		exit(1);
	}

	// Show Map
	// inFile->Map();

	// Retrieve the histogram from the file
	TH1F* histogram = dynamic_cast<TH1F*>(inFile->Get("EnergyADC/h_EBGO_ADC_2"));

	// Check if the histogram is retrieved successfully
	if (!histogram) {
		std::cerr << "Failed to retrieve histogram: " << "ADC2" << std::endl;
		inFile->Close();
		return;
	}

	histogram->Draw();
	c1->Update();

}