#include <analyzer.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include "run1776.h"
#include "utils.h"
#include <fstream>

/**
* @brief The constructor for the Analyzer object
* 
*/

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
	
	// Function type selector
	if (ftype == F1)
	{
		func = new TF1("f1", "[0]*x + [1] + [2]*exp(-0.5*((x-[3])/[4])^2)", chn_lower_bound, chn_upper_bound); // gaus is the same exact function
	}
	else if (ftype == F2)
	{
		func = new TF1("f1", "[0]*x + [1] + [2]*exp(-0.5*((x-[3])/[4])^2) + [5]*exp(-0.5*((x-[6])/[7])^2)", chn_lower_bound, chn_upper_bound);
	}
	else if (ftype == F3)
	{
		func = new TF1("f1", "[0]*x + [1] + [2]*exp(-0.5*((x-[3])/[4])^2) + [5]*exp(-0.5*((x-[6])/[7])^2) + [8]*exp(-0.5*((x-[9])/[10])^2)", chn_lower_bound, chn_upper_bound);
	}
}

/*
* @brief The destructor
* Doesn't close file otherwise the canvas gets wiped
*/

Analyzer::~Analyzer() {
	//if (inFile) inFile->Close();
	delete func;
	//delete canvas;
}

/** 
* @brief Sets fit parameters for gaussian + linear function
* 
* @param p0 Angular coefficient
* @param p1 Y-intercept
* @param p2 Normalization constant
* @param p3 Gaussian mean value
* @param p4 Gaussian standard deviation
*/

void Analyzer::setFitParameters(double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7, double p8, double p9, double p10) {
	if (ftype == F1) {
		func->SetParName(0, "Slope");
		func->SetParName(1, "Y-intercept");
		func->SetParName(2, "Normalization");
		func->SetParName(3, "Mean value");
		func->SetParName(4, "Standard Deviation");
		func->SetParameter(0, p0);
		func->SetParameter(1, p1);
		func->SetParameter(2, p2);
		func->SetParameter(3, p3);
		func->SetParameter(4, p4);
	}
	else if (ftype == F2) {
		func->SetParName(0, "Slope");
		func->SetParName(1, "Y-intercept");
		func->SetParName(2, "Normalization 1");
		func->SetParName(3, "Mean value 1");
		func->SetParName(4, "Standard Deviation 1");
		func->SetParName(5, "Normalization 2");
		func->SetParName(6, "Mean value 2");
		func->SetParName(7, "Standard Deviation 2");
		func->SetParameter(0, p0);
		func->SetParameter(1, p1);
		func->SetParameter(2, p2);
		func->SetParameter(3, p3);
		func->SetParameter(4, p4);
		func->SetParameter(5, p5);
		func->SetParameter(6, p6);
		func->SetParameter(7, p7);
	}
	else if (ftype == F3) {
		func->SetParName(0, "Slope");
		func->SetParName(1, "Y-intercept");
		func->SetParName(2, "Normalization 1");
		func->SetParName(3, "Mean value 1");
		func->SetParName(4, "Standard Deviation 1");
		func->SetParName(5, "Normalization 2");
		func->SetParName(6, "Mean value 2");
		func->SetParName(7, "Standard Deviation 2");
		func->SetParName(8, "Normalization 3");
		func->SetParName(9, "Mean value 3");
		func->SetParName(10, "Standard Deviation 3");
		func->SetParameter(0, p0);
		func->SetParameter(1, p1);
		func->SetParameter(2, p2);
		func->SetParameter(3, p3);
		func->SetParameter(4, p4);
		func->SetParameter(5, p5);
		func->SetParameter(6, p6);
		func->SetParameter(7, p7);
		func->SetParameter(8, p8);
		func->SetParameter(9, p9);
		func->SetParameter(10, p10);
	}
	
}

/**
* @brief Calculates efficiency with the peak area method
* 
* @param m How many more bins to consider when computing the efficiency (lowers uncertainty)
* 
*/

void Analyzer::efficiency(int m) {
	double integral = histogram->Integral(histogram->FindBin(chn_lower_bound), histogram->FindBin(chn_upper_bound));
	
	double area = (histogram->GetBinContent(histogram->FindBin(chn_lower_bound) - 1 - m) + histogram->GetBinContent(histogram->FindBin(chn_upper_bound) + 1 + m)) * (chn_upper_bound - chn_lower_bound) / 2;
	
	double eff = (integral - area) / (histogram->GetEntries());

	double err_peak = (std::sqrt(integral + area * (1 + (chn_upper_bound - chn_lower_bound) / (2 * m))));

	double std_dev = err_peak / (histogram->GetEntries());

	effic = eff;
	err_effic = std_dev;

	std::cout << "                                                                     " << std::endl;
	std::cout << "                                                                     " << std::endl;
	std::cout << "=====================================================================" << std::endl;
	std::cout << "Peak area (N counts):                     " << (integral - area) << "   +/-   " << err_peak << std::endl;
	//std::cout << "Efficiency:                     " << eff << "   +/-   " << std_dev << std::endl;
}

/**
* @brief Plot function for fitting the function, drawing onto the canvas both the histogram and the function fitted
*/
void Analyzer::plot() {
	histogram->Fit("f1", "", "", chn_lower_bound, chn_upper_bound);
	
	func->Draw();
	histogram->Draw();
	canvas->Update();
	
	std::cout << "p-value:                        " << func->GetProb() << std::endl;
}

/**
* @brief Function used to set upper and lower bound for fitting the function and calculating peak area
*/
void Analyzer::setUpperLowerBound(int chn_low, int chn_up) {
	chn_lower_bound = chn_low;
	chn_upper_bound = chn_up;
}


/**
* @brief Function used to get canvas
*/
TCanvas* Analyzer::getCanvas() const {
	return canvas;
}


/**
*@brief Function used to save results to a .txt file in the /out folder
*/
void Analyzer::saveResults() {

	size_t lastSlashPos_f = filename.find_last_of("/");
	size_t lastDotPos_f = filename.find_last_of(".");
	std::string outputFileName_f = filename.substr(lastSlashPos_f + 1, lastDotPos_f - lastSlashPos_f - 1);

	size_t lastSlashPos_h = histname.find_last_of("/");
	std::string outputFileName_h = histname.substr(lastSlashPos_h + 1, -1);

	std::string outputFilePath = "../../../out/" + outputFileName_f + "_" + outputFileName_h + ".txt";
	
	std::ofstream outFile(outputFilePath);
	if (!outFile.is_open()) {
		std::cerr << "Unable to open output file: " << outputFilePath << std::endl;
		return;
	}

	if (func) {
		outFile << "******************************************\n";
		outFile << "Fit Results:\n\n";
		for (int i = 0; i < func->GetNpar(); ++i) {
			outFile << func->GetParName(i) << " : " << func->GetParameter(i) << " +/- " << func->GetParError(i) << "\n";
		}
	}

	outFile << "Fit p-value: " << func->GetProb() << "\n";

	outFile << "Efficiency value: " << effic << " +- " << err_effic << "\n";

	outFile << "******************************************\n\n\n\n\n\n\n\n";
	outFile.close();

	std::cout << "Results save to " << outputFilePath << std::endl;
}


double Analyzer::getFitParameter(int index) {
	return func->GetParameter(index);
}

double Analyzer::getFitParameterError(int index) {
	return func->GetParError(index);
}
