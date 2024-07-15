#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include "run1776.h"
#include "utils.h"

void run1776 ()
{
	TCanvas* c1 = new TCanvas();

	TFile* inFile = new TFile("../../../root files/run1776_coinc.root", "read");

	if (!inFile || inFile->IsZombie())
	{
		std::cerr << "Failed to open file: " << "run1776_coinc.root" << std::endl;
		exit(1);
	}

	// Retrieve the histogram from the file
	TH1F* histogram = dynamic_cast<TH1F*>(inFile->Get("EnergyADC/h_EBGO_ADC_2"));

	// Check if the histogram is retrieved successfully
	if (!histogram) {
		std::cerr << "Failed to retrieve histogram: " << "ADC2" << std::endl;
		inFile->Close();
		return;
	}

	// Fit a linear function + gaussian function

	
	TF1* func = new TF1("f1", "[0]*x + [1] + [2]*exp(-0.5*((x-[3])/[4])^2)/([3]*sqrt(2*pi))", 300, histogram->GetMaximum());
	func->SetParameter(0, 1/2);
	func->SetParameter(1, 400);
	func->SetParameter(2, 1);
	func->SetParameter(3, 550);
	func->SetParameter(4, 44);

	TF1* func2 = new TF1("f2", "[0]*exp(-0.5*((x-[1])/[2])^2)");

	func2->SetParameter(0, 2);
	func2->SetParameter(1, 559);
	func2->SetParameter(2, 50);

	double chn_1 = 450;
	double chn_2 = 650;
	int m = 15;

    histogram->Fit("f1", "", "", chn_1, chn_2);
	//histogram->Fit("f2", "", "", 450, 650);
	double integral = histogram->Integral(histogram->FindBin(chn_1), histogram->FindBin(chn_2));
	double area = trap_area(histogram, chn_1, chn_2, m);
	double eff = (integral - area)/(histogram->GetEntries());

	double std_dev = var_peak(area, integral, (chn_2 - chn_1), m) / histogram->GetEntries();

	std::cout << "p-value:                        " << func->GetProb() << std::endl;
	std::cout << "Efficiency:                        " << eff << "   +/-   " << std_dev << std::endl;

	func2->Draw();
	histogram->Draw();
	c1->Update();

}