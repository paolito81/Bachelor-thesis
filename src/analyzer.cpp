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

/**
* @brief The constructor for the Analyzer object
* 
*/
Analyzer::Analyzer(const std::string& filename, const std::string& histname, FuncType ftype) :
	filename(filename), histname(histname), inFile(nullptr), histogram(nullptr), func(nullptr), canvas(nullptr), ftype(ftype),
	p0(0), p1(0), p2(0), p3(0), p4(0), p5(0), p6(0), p7(0), p8(0), p9(0), p10(0),
	chn_lower_bound(0), chn_upper_bound(1),
	livetime(0), err_livetime(0), activity(0), err_activity(0)
{

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
	else if (ftype == F4)
	{
		func = new TF1("f1", "[0]*x + [1] + gausn(2)", chn_lower_bound, chn_upper_bound);
	}
	else if (ftype == F5)
	{
		func = new TF1("f1", "[0]*x + [1] + gausn(2) + gausn(5)", chn_lower_bound, chn_upper_bound);
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
	
	func->SetParameter(0, p0);
	func->SetParameter(1, p1);
	func->SetParameter(2, p2);
	func->SetParameter(3, p3);
	func->SetParameter(4, p4);

	switch (ftype)
	{
	case F1:
	case F4:
		func->SetParName(0, "Slope");
		func->SetParName(1, "Y-intercept");
		func->SetParName(2, "Normalization (peak area if F4)");
		func->SetParName(3, "Mean value");
		func->SetParName(4, "Standard Deviation");
		break;
	case F2:
	case F5:
		func->SetParName(0, "Slope");
		func->SetParName(1, "Y-intercept");
		func->SetParName(2, "Normalization 1 (peak area if F5");
		func->SetParName(3, "Mean value 1");
		func->SetParName(4, "Standard Deviation 1");
		func->SetParName(5, "Normalization 2 (peak area if F5");
		func->SetParName(6, "Mean value 2");
		func->SetParName(7, "Standard Deviation 2");
		func->SetParameter(5, p5);
		func->SetParameter(6, p6);
		func->SetParameter(7, p7);
		break;
	case F3:
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
		func->SetParameter(5, p5);
		func->SetParameter(6, p6);
		func->SetParameter(7, p7);
		func->SetParameter(8, p8);
		func->SetParameter(9, p9);
		func->SetParameter(10, p10);
		break;
	default:
		std::cout << "Invalid function type!" << std::endl;
		break;
	}
}

/**
* @brief Calculates efficiency with the peak area method, only works for F1 and F4
* 
* @param m How many more bins to consider when computing the efficiency (lowers uncertainty)
* 
*/
void Analyzer::trapefficiency(int m) {
	if (ftype == F1 || ftype == F4) {
		double integral = histogram->Integral(histogram->FindBin(chn_lower_bound), histogram->FindBin(chn_upper_bound));

		double area = (histogram->GetBinContent(histogram->FindBin(chn_lower_bound) - 1 - m) + histogram->GetBinContent(histogram->FindBin(chn_upper_bound) + 1 + m)) * (chn_upper_bound - chn_lower_bound) / 2;

		double effic = (integral - area) / (activity*livetime);

		double err_peak = (std::sqrt(integral + area * (1 + (chn_upper_bound - chn_lower_bound) / (2 * m))));

		double err_effic = std::sqrt(err_peak*err_peak + err_activity*err_activity + err_livetime*err_livetime);

		//what were these for???
		//effic = eff;
		//err_effic = std_dev;

		std::cout << "\n\n" << std::endl;
		std::cout << "=====================================================================" << std::endl;
		std::cout << "Peak area (N counts):                     " << (integral - area) << "   +/-   " << err_peak << std::endl;
		std::cout << "Efficiency (trap):                     " << effic << "   +/-   " << err_effic << std::endl;
	}
	else {
		std::cout << "\n\n" << std::endl;
		std::cout << "Couldn't calculate efficiency with the peak area method, the function type is not F1 or F4." << std::endl;
	}
}

/**
 * @brief Calculates efficiency with the peak area method, only works for F4 and F5
 */
void Analyzer::normefficiency() {
	
	double eff1 = func->GetParameter(2) / (activity * livetime);
    //EFFICIENCY ERROR
	if (ftype == F4) {
		std::cout << "\n\n" << std::endl;
		std::cout << "=====================================================================" << std::endl;
		std::cout << "Efficiency (norm):                     " << eff1 << "   +/-   " << std::endl;
	}
	if (ftype == F5) {
		double eff2 = func->GetParameter(5);
		std::cout << "\n\n" << std::endl;
		std::cout << "=====================================================================" << std::endl;
		std::cout << "Efficiency 1 (norm):                     " << eff1 << "   +/-   " << std::endl;
		std::cout << "Efficiency 2 (norm):                     " << eff2 << "   +/-   " << std::endl;
	}
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

	std::string outputFilePath = "../../../out/fit results/" + outputFileName_f + "_" + outputFileName_h + ".txt";
	
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

	//outFile << "Efficiency value: " << effic << " +- " << err_effic << "\n";

	outFile << "******************************************";
	outFile.close();

	std::cout << "Results saved to " << outputFilePath << std::endl;
	std::cout << "\n\n";
}

/**
 * @brief function to get fit parameter
 * @param index the index to the fit parameter
 * @return the fit parameter
 */
double Analyzer::getFitParameter(int index) {
	return func->GetParameter(index);
}

/**
 * @brief function to get fit parameter error
 * @param index the index to the fit parameter
 * @return the error to the fit parameter
 */
double Analyzer::getFitParameterError(int index) {
	return func->GetParError(index);
}

/**
 * @brief Function to calculate the live time of the counting experiment  
 */
/*void Analyzer::pulser() {// pulser always falls around 4500, the coincidence is between 2600 and 3500
	
	std::string pulsername = "EnergyADC/h_EBGO_ADC_0";
	TH1F* pulser = dynamic_cast<TH1F*>(inFile->Get(pulsername.c_str()));

	double pulsercounts = pulser->Integral(pulser->FindBin(4400), pulser->FindBin(4600));

	double histocounts = histogram->Integral(histogram->FindBin(2600), histogram->FindBin(3500));

	livetime = 1 + histocounts / pulsercounts;

	//std::cout << "=====================================================================" << std::endl;
	std::cout << "Live time:                     " << livetime << "   +/-   " << std::endl;
	std::cout << "\n\n";
}*/

void Analyzer::pulser() {// pulser always falls around 4500, the coincidence is between 2600 and 3500

	TTree* tree = dynamic_cast<TTree*>(inFile->Get("T_coinc"));

	if (!tree) {
		std::cerr << "Error: could not find TTree in the file!" << std::endl;
	}

	//tree->Print();

	short channel[8];
	TH1F* h_pulser_energycoinc_BGO[6] = { nullptr };

	for (int i = 0; i < 6; ++i) {
		h_pulser_energycoinc_BGO[i] = new TH1F(Form("hist_%d", i), Form("Histogram %d", i), 4000, 0, 4000);
	}

	// Set the branch address to read the "Channel" branch into the `channel` array
	tree->SetBranchAddress("Channel", channel);

	for (int i = 0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);  // Get the current entry

		if (channel[0] > 0 && channel[1] > 2600 && channel[1] < 3500)
			h_pulser_energycoinc_BGO[0]->Fill(channel[1]);
		if (channel[0] > 0 && channel[2] > 2600 && channel[2] < 3500)
			h_pulser_energycoinc_BGO[1]->Fill(channel[2]);
		if (channel[0] > 0 && channel[3] > 2600 && channel[3] < 3500)
			h_pulser_energycoinc_BGO[2]->Fill(channel[3]);
		if (channel[0] > 0 && channel[4] > 2600 && channel[4] < 3500)
			h_pulser_energycoinc_BGO[3]->Fill(channel[4]);
		if (channel[0] > 0 && channel[5] > 2600 && channel[5] < 3500)
			h_pulser_energycoinc_BGO[4]->Fill(channel[5]);
		if (channel[0] > 0 && channel[6] > 2600 && channel[6] < 3500)
			h_pulser_energycoinc_BGO[5]->Fill(channel[6]);
	}

	TCanvas* canvas = new TCanvas("histos", "Canvas with all histos", 1200, 800);
	canvas->Divide(3, 2);

	for (int i = 0; i < 6; ++i) {
		canvas->cd(i + 1);
		if (h_pulser_energycoinc_BGO[i]) {
			h_pulser_energycoinc_BGO[i]->Draw();
		}
		else {
			std::cerr << "Histogram " << i << " is null" << std::endl;
		}
	}

	canvas->Update();
	canvas->SaveAs("../../../out/coinc hist/Histos.pdf");
	//canvas->Close();

	float coinc_event[6];

	for (int i = 0; i < 6; ++i) {
		coinc_event[i] = h_pulser_energycoinc_BGO[i]->Integral(h_pulser_energycoinc_BGO[i]->FindBin(2600), h_pulser_energycoinc_BGO[i]->FindBin(3500));
		
		std::cout << "Integral pulser and BGO" << i << ": " << coinc_event[i] << std::endl;
	}

	for (int i = 0; i < 6; ++i) {
		double time_perc = coinc_event[i] / coinc_event[0];
		std::cout << "Live time percetage for CHN" << i << ": " << time_perc << std::endl;
	}



	/*
	short channel[8];
	

	tree->SetBranchAddress("Channel", channel);

	for (int i = 0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);

		if (channel[0] > 0 && channel[1] > 2600 && channel[1] < 3500)
			h_pulser_energycoinc_BGO[1]->Fill(channel[1]);
		if (channel[0] > 0 && channel[2] > 2600 && channel[2] < 3500)
			h_pulser_energycoinc_BGO[2]->Fill(channel[2]);
		if (channel[0] > 0 && channel[3] > 2600 && channel[3] < 3500)
			h_pulser_energycoinc_BGO[3]->Fill(channel[3]);
		if (channel[0] > 0 && channel[4] > 2600 && channel[4] < 3500)
			h_pulser_energycoinc_BGO[4]->Fill(channel[4]);
		if (channel[0] > 0 && channel[5] > 2600 && channel[5] < 3500)
			h_pulser_energycoinc_BGO[5]->Fill(channel[5]);
		if (channel[0] > 0 && channel[6] > 2600 && channel[6] < 3500)
			h_pulser_energycoinc_BGO[6]->Fill(channel[6]);
	}

	float coinc_event[8];

	for (int i = 0; i < 8; ++i) {
		coinc_event[i] = 0;
	}

	for (int i = 1; i < 7; ++i) {
		coinc_event[i] = h_pulser_energycoinc_BGO[i]->Integral(2600, 3500);

		std::cout << "Integral pulser and BGO" << i << ": " << coinc_event[i] << std::endl;
	}*/



}

void Analyzer::setActivity() {
	
	switch (ftype)
	{
	case F1:
	case F4:
		activity = 6460 * exp(-getHowManyYears("25/07/2016") / 30.05);
		err_activity = 70;
		break;
	case F2:
	case F5:
		activity = 9010 * exp(-getHowManyYears("01/07/2016") / 5.27);
	}

	
	
}

void Analyzer::printActivity() {
	std::cout << "The activity is: " << activity << " Bq" << std::endl;
}