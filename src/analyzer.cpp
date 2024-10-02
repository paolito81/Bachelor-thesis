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

/**
* @brief The constructor for the Analyzer object
* 
*/
Analyzer::Analyzer(const std::string& filename, const std::string& histname, FuncType ftype) :
	filename(filename), histname(histname), inFile(nullptr), histogram(nullptr), func(nullptr), canvas(nullptr), ftype(ftype),
	p0(0), p1(0), p2(0), p3(0), p4(0), p5(0), p6(0), p7(0), p8(0), p9(0), p10(0),
	chn_lower_bound(0), chn_upper_bound(1),
	activity(0), err_activity(0), total_time(0), err_total_time(0), time_perc(0),
	pulser_integral(0),
	efficiency1(0), efficiency2(0), err_efficiency1(0), err_efficiency2(0), trap_efficiency(0), err_trap_efficiency(0)
{

	inFile = new TFile(filename.c_str(), "read");

	if (!inFile || inFile->IsZombie())
	{
		std::cerr << "Failed to open file: " << filename.c_str() << std::endl;
		exit(1);
	}

	// Retrieve the histogram from the file
	histogram = dynamic_cast<TH1F*>(inFile->Get(histname.c_str()));

	// Calculate pulser integral
	TH1F* pulser_hist = dynamic_cast<TH1F*>(inFile->Get("EnergyADC/h_EBGO_ADC_0"));
	pulser_integral = pulser_hist->Integral(pulser_hist->FindBin(4000), pulser_hist->FindBin(5000));

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
		func = new TF1("f1", "[0]*x + [1] + gausn(2) + gausn(5) + [8]*x^2", chn_lower_bound, chn_upper_bound);
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
		func->SetParName(2, "Normalization 1");
		func->SetParName(3, "Mean value 1");
		func->SetParName(4, "Standard Deviation 1");
		break;
	case F2:
	case F5:
		func->SetParName(0, "Slope");
		func->SetParName(1, "Y-intercept");
		func->SetParName(2, "Normalization 1");
		func->SetParName(3, "Mean value 1");
		func->SetParName(4, "Standard Deviation 1");
		func->SetParName(5, "Normalization 2");
		func->SetParName(6, "Mean value 2");
		func->SetParName(7, "Standard Deviation 2");
		func->SetParameter(5, p5);
		func->SetParameter(6, p6);
		func->SetParameter(7, p7);
		break;
	case F3:
		func->SetParName(0, "Slope");
		func->SetParName(1, "Y-intercept");
		func->SetParName(2, "Normalization 1");
		func->SetParName(3, "Mean value 1");
		func->SetParName(4, "Standard Deviation 1");
		func->SetParName(5, "Normalization 2");
		func->SetParName(6, "Mean value 2");
		func->SetParName(7, "Standard Deviation 2");
		func->SetParName(8, "Quadratic term");
		func->SetParameter(5, p5);
		func->SetParameter(6, p6);
		func->SetParameter(7, p7);
		func->SetParameter(8, p8);
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
		double err_integral = sqrt(integral);
		
		double area = (chn_upper_bound - chn_lower_bound) * (histogram->Integral(histogram->FindBin(chn_lower_bound - m), histogram->FindBin(chn_lower_bound - 1)) +
			histogram->Integral(histogram->FindBin(chn_upper_bound + 1), histogram->FindBin(chn_upper_bound + m))) / (2 * m);
		
		double peak = integral - area;

		trap_efficiency = peak / (activity*total_time*time_perc);

		double err_peak = (sqrt(integral + area * (1 + (chn_upper_bound - chn_lower_bound) / (2 * m))));

		err_trap_efficiency = trap_efficiency * sqrt((err_peak / peak)*(err_peak/peak) + (err_activity/activity)* (err_activity / activity)) / (total_time*time_perc);

		std::cout << "\n\n" << std::endl;
		std::cout << "=====================================================================" << std::endl;
		std::cout << "Peak area (N counts):                     " << peak << "   +/-   " << err_peak << std::endl;
		std::cout << "Efficiency (trap):                     " << trap_efficiency << "   +/-   " << err_trap_efficiency << std::endl;
	}
	else {
		std::cout << "\n\n" << std::endl;
		std::cout << "Couldn't calculate efficiency with the peak area method, the function type is not F1 or F4." << std::endl;
	}
}
/*
void Analyzer::normefficiency_redux(double EL, double ER, double E1L, double E2L, double E1R, double E2R) {
	
	if (ftype == F1 || ftype == F4) {

	int binLeft = histogram->FindBin(EL);
	int binRight = histogram->FindBin(ER);
	int binBackgroundLeft1 = histogram->FindBin(E1L);
	int binBackgroundLeft2 = histogram->FindBin(E2L);
	int binBackgroundRight1 = histogram->FindBin(E1R);
	int binBackgroundRight2 = histogram->FindBin(E2R);

	double PeakRawSum = histogram->Integral(binLeft, binRight);
	double err_PeakRawSum = sqrt(PeakRawSum);

	double backgroundLeft = histogram->Integral(binBackgroundLeft1, binBackgroundLeft2);
	double backgroundRight = histogram->Integral(binBackgroundRight1, binBackgroundRight2);

	int binNumberLeft = binBackgroundLeft2 - binBackgroundLeft1 + 1;
	int binNumberRight = binBackgroundRight2 - binBackgroundRight2 + 1;
	int binNumberPeak = binRight - binLeft + 1;

	double dP = 0;
	double peak_centroid = 0;
	for (int i = binLeft; i <= binRight; ++i) {
		dP = histogram->GetBinContent(i);
		peak_centroid += dP * i;
	}

	double dBackgroundLeft = 0;
	double left_background_centroid = 0;
	for (int i = binBackgroundLeft1; i <= binBackgroundLeft2; ++i) {
		dBackgroundLeft = histogram->GetBinContent(i);
		left_background_centroid += dBackgroundLeft * i;
	}

	double dBackgroundRight = 0;
	double right_background_centroid = 0;
	for (int i = binBackgroundRight1; i <= binBackgroundRight2; ++i) {
		dBackgroundRight = histogram->GetBinContent(i);
		right_background_centroid += dBackgroundRight * i;
	}

	double distanceLeft = std::fabs(peak_centroid - left_background_centroid);
	double distanceRight = std::fabs(peak_centroid - right_background_centroid);

	double weightLeft = distanceLeft / (distanceLeft + distanceRight);
	double weightRight = distanceRight / (distanceLeft + distanceRight);

	//che cos'� f??
	double f = backgroundLeft / binNumberLeft * weightLeft + backgroundRight / binNumberRight * weightRight;
	double background = f * binNumberPeak;

	double err_background = sqrt(background);

	double PeakNetSum = PeakRawSum - background;
	double err_PeakNetSum = sqrt(pow(err_PeakRawSum, 2) + pow(err_background, 2));


		std::cout << "\n\n" << std::endl;
		std::cout << "=====================================================================" << std::endl;
		std::cout << "Peak area (N counts):                     " << PeakNetSum << "   +/-   " << err_PeakNetSum << std::endl;
		std::cout << "Efficiency (trap):                     " << trap_efficiency << "   +/-   " << err_trap_efficiency << std::endl;
	}

}
*/

void Analyzer::trapefficiency_redux(int m) {

	if (ftype == F1 || ftype == F4) {

		int binLeft = histogram->FindBin(chn_lower_bound);
		int binRight = histogram->FindBin(chn_upper_bound);
		int binBackgroundLeft1 = histogram->FindBin(chn_lower_bound - 1 - m);
		int binBackgroundLeft2 = histogram->FindBin(chn_lower_bound - 1);
		int binBackgroundRight1 = histogram->FindBin(chn_upper_bound + 1);
		int binBackgroundRight2 = histogram->FindBin(chn_upper_bound + 1 + m);

		double PeakRawSum = histogram->Integral(binLeft, binRight);
		double err_PeakRawSum = sqrt(PeakRawSum);

		double backgroundLeft = histogram->Integral(binBackgroundLeft1, binBackgroundLeft2);
		double backgroundRight = histogram->Integral(binBackgroundRight1, binBackgroundRight2);

		int binNumberLeft = binBackgroundLeft2 - binBackgroundLeft1 + 1;
		int binNumberRight = binBackgroundRight2 - binBackgroundRight1 + 1;
		int binNumberPeak = binRight - binLeft + 1;

		double dP = 0;
		double peak_centroid = 0;
		for (int i = binLeft; i <= binRight; ++i) {
			dP = histogram->GetBinContent(i);
			peak_centroid += dP * i;
		}

		double dBackgroundLeft = 0;
		double left_background_centroid = 0;
		for (int i = binBackgroundLeft1; i <= binBackgroundLeft2; ++i) {
			dBackgroundLeft = histogram->GetBinContent(i);
			left_background_centroid += dBackgroundLeft * i;
		}

		double dBackgroundRight = 0;
		double right_background_centroid = 0;
		for (int i = binBackgroundRight1; i <= binBackgroundRight2; ++i) {
			dBackgroundRight = histogram->GetBinContent(i);
			right_background_centroid += dBackgroundRight * i;
		}

		double distanceLeft = std::fabs(peak_centroid - left_background_centroid);
		double distanceRight = std::fabs(peak_centroid - right_background_centroid);

		double weightLeft = distanceLeft / (distanceLeft + distanceRight);
		double weightRight = distanceRight / (distanceLeft + distanceRight);

		//che cos'� f??
		double f = backgroundLeft / binNumberLeft * weightLeft + backgroundRight / binNumberRight * weightRight;
		double background = f * binNumberPeak;

		double err_background = sqrt(background);

		double PeakNetSum = PeakRawSum - background;
		double err_PeakNetSum = sqrt(pow(err_PeakRawSum, 2) + pow(err_background, 2));

		trap_efficiency = PeakNetSum / (activity * total_time * time_perc);
		err_trap_efficiency = trap_efficiency * sqrt(pow(err_PeakNetSum/PeakNetSum, 2) + pow(err_activity/activity, 2) + pow(err_total_time/total_time, 2))/time_perc;

		std::cout << "\n\n" << std::endl;
		std::cout << "=====================================================================" << std::endl;
		std::cout << "Peak area (N counts):                     " << PeakNetSum << "   +/-   " << err_PeakNetSum << std::endl;
		std::cout << "Efficiency (trap):                     " << trap_efficiency << "   +/-   " << err_trap_efficiency << std::endl;
	}
	else if (ftype == F2 || ftype == F5 || ftype == F3) {
		std::cout << "\n\n" << std::endl;
		std::cout << "Couldn't calculate efficiency with the peak area method, the function type is not F1 or F4." << std::endl;
	}

}

/**
 * @brief Calculates efficiency with the fit parameter method, only works for F4 and F5
 */
void Analyzer::normefficiency() {
	
	efficiency1 = func->GetParameter(2) / (activity * total_time * time_perc);
	err_efficiency1 = efficiency1 * sqrt(pow(func->GetParError(2) / func->GetParameter(2), 2) + pow(err_activity / activity, 2) + pow(err_total_time / total_time, 2))/time_perc;
	
	if (ftype == F4) {
		std::cout << "\n\n" << std::endl;
		std::cout << "=====================================================================" << std::endl;
		std::cout << "Efficiency 1 (norm):                     " << efficiency1 << "   +/-   " << err_efficiency1 << std::endl;
	}
	if (ftype == F3 || ftype == F5) {
		efficiency2 = func->GetParameter(5) / (activity * total_time * time_perc);
		err_efficiency2 = efficiency2 * sqrt(pow(func->GetParError(5) / func->GetParameter(5), 2) + pow(err_activity / activity, 2) + pow(err_total_time/total_time, 2)) / time_perc;

		std::cout << "\n\n" << std::endl;
		std::cout << "=====================================================================" << std::endl;
		std::cout << "Efficiency 1 (norm):                     " << efficiency1 << "   +/-   " << err_efficiency1 << std::endl;
		std::cout << "Efficiency 2 (norm):                     " << efficiency2 << "   +/-   " << err_efficiency2 <<  std::endl;
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
	
	std::string folder = "out/fit results/";
	std::string outputFilePath = getOutputFilePath(folder, filename, histname) + ".txt";

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

	outFile << "Fit p-value: " << func->GetProb() << "\n\n";
	outFile << "Pulser integral: " << pulser_integral << "\n";
	outFile << "Total time for measurements: " << total_time << " s" << "\n";
	outFile << "Live time percentage: " << time_perc << "\n\n";
	outFile << "Activity for the source today: " << activity << " Bq" << "\n";

	if (ftype == F4) {
		outFile << "Efficiency value for first peak: " << efficiency1 << " +/- " << err_efficiency1 << "\n";
		outFile << "Efficiency value with trap method: " << trap_efficiency << " +/- " << err_trap_efficiency << "\n";
	}
	if (ftype == F5 || ftype == F3) {
		outFile << "Efficiency value for first peak: " << efficiency1 << " +/- " << err_efficiency1 << "\n";
		outFile << "Efficiency value for second peak: " << efficiency2 << " +/- " << err_efficiency2 << "\n";
	}

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
 * @brief Function to calculate the live time percentage of the counting experiment
 * @param pulser_min The lower bound of the region of interest for the coincidence integral
 * @param pulser_max The upper bound of the region of interest for the coincidence integral
 */
void Analyzer::pulser(int pulser_min, int pulser_max) {// pulser always falls around 4500, the coincidence is between 2600 and 3500

	TTree* tree = dynamic_cast<TTree*>(inFile->Get("T_coinc"));

	if (!tree) {
		std::cerr << "Error: could not find TTree in the file!" << std::endl;
	}

	int index = extractLastNumber(histname);
	std::string name = histname + " coinc";
	
	short channel[8];
	TH1F* h_pulser_energycoinc_BGO = new TH1F(name.c_str(), name.c_str(), (pulser_max - pulser_min), pulser_min, pulser_max);
	gStyle->SetOptStat(0);
	h_pulser_energycoinc_BGO->SetFillColor(kAzure - 9);
	h_pulser_energycoinc_BGO->SetLineColor(kBlack);
	h_pulser_energycoinc_BGO->SetLineWidth(2);

	// Set the branch address to read the "Channel" branch into the `channel` array
	tree->SetBranchAddress("Channel", channel);

	for (int i = 0; i < tree->GetEntries(); ++i) {
		tree->GetEntry(i);  // Get the current entry

		if (channel[0] > 0 && channel[index] > pulser_min && channel[index] < pulser_max)
			h_pulser_energycoinc_BGO->Fill(channel[index]);
	}

	TCanvas* canvas = new TCanvas(Form("histogram", histname), Form("Coinc. Histogram for", histname), 1200, 800);


	if (h_pulser_energycoinc_BGO) {
		h_pulser_energycoinc_BGO->Draw();
	}
	else {
		std::cerr << "Histogram " << index << " is null" << std::endl;
	}

	std::string folder = "out/coinc hist/";
	std::string saveName = getOutputFilePath(folder, filename, histname) + "_COINC" + ".pdf";

	canvas->Update();
	canvas->SaveAs(saveName.c_str());

	float coinc_event = h_pulser_energycoinc_BGO->Integral(h_pulser_energycoinc_BGO->FindBin(pulser_min), h_pulser_energycoinc_BGO->FindBin(pulser_max));
	time_perc = coinc_event / pulser_integral;

	std::cout << "Integral pulser for BGO" << index << ": " << coinc_event << std::endl;
	std::cout << "Live time percetage for CHN" << index << ": " << time_perc << std::endl;
}

/**
 * @brief A function to set the activity for the source, on this day
 */
void Analyzer::setActivity() {
	
	switch (ftype)
	{
	case F1:
	case F4:
		activity = 0.85 * 6460 * exp(-getHowManyYears("25/07/2016") / (30.08/0.693));
		err_activity = 0.85 * 70 * exp(-getHowManyYears("25/07/2016") / (30.08 / 0.693));
		break;
	case F2:
	case F3:
	case F5:
		activity = 9010 * exp(-getHowManyYears("01/07/2016") / (5.27/0.693));
		err_activity = 70 * exp(-getHowManyYears("01/07/2016") / (5.27 / 0.693));
	}
}

/**
 * @brief A function to set the total time, the time for which measurements are taken 
 */
void Analyzer::setTotalTime() {
	err_total_time = 1;
	if (filename == "../../../root files/run1776_coinc.root") {
		total_time = 463;
	}
	if (filename == "../../../root files/run1777_coinc.root") {
		total_time = 173;
	}
	if (filename == "../../../root files/run1778_coinc.root") {
		total_time = 0;
	}
	if (filename == "../../../root files/run1779_coinc.root") {
		total_time = 156;
	}
	if (filename == "../../../root files/run1780_coinc.root") {
		total_time = 170;
	}
}

/**
 * @brief A function to print the activity class member
 */
void Analyzer::printActivity() const {
	std::cout << "The activity is: " << activity << " Bq" << std::endl;
}

/**
 * @brief A function to compute a Z Test between the efficiency values calculated through the trapezoid method and the fit method
 */
void Analyzer::ZTestEfficiencies() const {

	if (ftype == F1 || ftype == F4) {
		double Z_stat = (efficiency1 - trap_efficiency) / sqrt(pow(err_efficiency1, 2) + pow(err_trap_efficiency, 2));
		double pvalue = 1 - 0.5 * TMath::Erfc(Z_stat / TMath::Sqrt(2));

		std::cout << "Efficiencies Z test: " << "\n";
		std::cout << "Z statistic: " << Z_stat << "\n";
		std::cout << "Z test p-value: " << pvalue << "\n";

		if (fabs(Z_stat) < 3) {
			std::cout << "Data is within 3 sigma of the expected distribution." << std::endl;
		}
	}
}