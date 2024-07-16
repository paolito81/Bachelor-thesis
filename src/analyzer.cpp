#include <analyzer.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include "run1776.h"
#include "utils.h"

Analyzer::Analyzer(const std::string& filename, const std::string& histname, FuncType ftype) :
	filename(filename), histname(histname), inFile(nullptr), histogram(nullptr), func(nullptr), canvas(nullptr), ftype(ftype) {

	inFile = new TFile(filename.c_str(), "read");

	if (!inFile || inFile->IsZombie())
	{
		std::cerr << "Failed to open file: " << filename.c_str() << std::endl;
		exit(1);
	}

	// Retrieve the histogram from the file
	histogram = dynamic_cast<TH1F*>(inFile->Get(histname.c_str()));

	// Check if the histogram is retrieved successfully
	if (!histogram) {
		std::cerr << "Failed to retrieve histogram: " << histname.c_str() << std::endl;
		inFile->Close();
		return;
	}

	canvas = new TCanvas();
	
	if (ftype == F1)
	{
		func = new TF1("f1", "[0]*x + [1] + [2]*exp(-0.5*((x-[3])/[4])^2)", chn_lower_bound, chn_upper_bound);
	}
	else if (ftype == F2)
	{
		func = new TF1("f1", "[0]*x + [1] + [4]*exp(-0.5*((x-[2])/[3])^2) + [7]*exp(-0.5*((x-[5])/[6])^2)", chn_lower_bound, chn_upper_bound);
	}
	
	

}

/*
* The destructor
* Doesn't close file otherwise the canvas gets wiped
*/

Analyzer::~Analyzer() {
	//if (inFile) inFile->Close();
	delete func;
	//delete canvas;
}

/** 
* Sets fit parameters for gaussian + linear function
* 
* @param p0 Angular coefficient
* @param p1 Y-intercept
* @param p2 Normalization constant
* @param p3 Gaussian mean value
* @param p4 Gaussian standard deviation
*/

void Analyzer::setFitParameters(double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7) {
	if (ftype == F1) {
		func->SetParameter(0, p0);
		func->SetParameter(1, p1);
		func->SetParameter(2, p2);
		func->SetParameter(3, p3);
		func->SetParameter(4, p4);
	}
	else if (ftype == F2) {
		func->SetParameter(0, p0);
		func->SetParameter(1, p1);
		func->SetParameter(2, p2);
		func->SetParameter(3, p3);
		func->SetParameter(4, p4);
		func->SetParameter(5, p5);
		func->SetParameter(6, p6);
		func->SetParameter(7, p7);
	}
	
}

/**
* Calculates efficiency with the peak area method
* 
* @param m How many more bins to consider when computing the efficiency (lowers uncertainty)
* 
*/

void Analyzer::efficiency(int m) {
	double integral = histogram->Integral(histogram->FindBin(chn_lower_bound), histogram->FindBin(chn_upper_bound));
	
	double area = (histogram->GetBinContent(histogram->FindBin(chn_lower_bound) - 1 - m) + histogram->GetBinContent(histogram->FindBin(chn_upper_bound) + 1 + m)) * (chn_upper_bound - chn_lower_bound) / 2;
	
	double eff = (integral - area) / (histogram->GetEntries());

	double std_dev = (std::sqrt(integral + area * (1 + (chn_upper_bound - chn_lower_bound) / (2 * m))))/ (histogram->GetEntries());

	std::cout << "                                                                     " << std::endl;
	std::cout << "                                                                     " << std::endl;
	std::cout << "=====================================================================" << std::endl;
	std::cout << "Efficiency:                     " << eff << "   +/-   " << std_dev << std::endl;
}

void Analyzer::plot() {
	histogram->Fit("f1", "", "", chn_lower_bound, chn_upper_bound);
	
	func->Draw();
	histogram->Draw();
	canvas->Update();
	
	std::cout << "p-value:                        " << func->GetProb() << std::endl;
}

void Analyzer::setUpperLowerBound(int chn_low, int chn_up) {
	chn_lower_bound = chn_low;
	chn_upper_bound = chn_up;
}

TCanvas* Analyzer::getCanvas() const {
	return canvas;
}