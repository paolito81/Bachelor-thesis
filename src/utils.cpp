#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <utils.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <filesystem>
#include <cmath>
#include <config.h>
#include <TGraphErrors.h>
#include <graphplotter.h>

const std::filesystem::path welcomeFilePath{ "../../../welcome.txt" };

//Used to check if TFile is open
void isTFileOpen(TFile* inFile) {
	if (!inFile || inFile->IsZombie())
	{
		std::cerr << "Failed to open file: " << inFile->GetName() << std::endl;
		exit(1);
	}
}

//Used to display .txt file on the terminal
void display(std::string filename) {
    std::string displaystr;
    std::ifstream inFile;

    inFile.open(welcomeFilePath);
    if (!inFile.is_open()) {
        std::cout << "Unable to open file";
        //throw std::runtime_error("Error opening welcome.txt");
        exit(1);
    }

    while (getline(inFile, displaystr)) {
        std::cout << displaystr << std::endl;
    }

    inFile.close();
}

// used to calculate background area under energy peak
static double trap_area(TH1F* histogram, int chn_1, int chn_2, int m) {
    double area = (histogram->GetBinContent(histogram->FindBin(chn_1) - 1 - m) + histogram->GetBinContent(histogram->FindBin(chn_2) + 1 + m)) * (chn_2 - chn_1) / 2;
    return area;
}

// used to calculate the uncertainty of the calculated peak area
static double var_peak(double area, double trap, int n, int m) {
    double variance = std::sqrt(area + trap*(1 + n/(2*m)));
    return variance;
}


//UNUSED
static void MakeGraphErrors(int configCount, int elementsPerVector, std::vector<double>& xValues, std::vector<double>& errxValues) {
    std::vector<double> yValues = { 661,1173.2,1332.5,2505.7 };
    std::vector<std::vector<double>> sep_xValues;
    std::vector<std::vector<double>> sep_errxValues;
    
    for (int i = 0; i < configCount / 3; ++i) {
        std::vector<double> xTemp;
        std::vector<double> xerrTemp;
        for (int j = 0; j < elementsPerVector; ++j) {
            xTemp.push_back(xValues[i * elementsPerVector + j]);
            xerrTemp.push_back(errxValues[i * elementsPerVector + j]);
        }
        sep_xValues.push_back(xTemp);
        sep_errxValues.push_back(xerrTemp);
    }

    for (int i = 0; i < configCount / 3; ++i) {
        TGraphErrors* graph = new TGraphErrors(xValues.size(), sep_xValues[i].data(), yValues.data(), sep_errxValues[i].data(), nullptr);
        std::string canvasName = "c" + std::to_string(i);
        TCanvas* c1 = new TCanvas(canvasName.c_str(), "meanplots", 800, 600);
        /*
        TF1* fourpoly = new TF1("fourpoly", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4", 0, 2300);
        fourpoly->SetParameters(0, 0, 0, 0, 0);
        */
        TF1* linear = new TF1("fourpoly", "[0] + [1]*x", 0, 2300);
        linear->SetParameters(0, 1);

        graph->SetTitle("grapho");
        graph->GetYaxis()->SetTitle("Energy [keV]");
        graph->GetXaxis()->SetTitle("Channels [CHN]");
        graph->SetMarkerStyle(21);
        graph->Draw("AP");

        graph->Fit("fourpoly", "R");
        //fourpoly->GetProb();
        std::cout << "P-value: " << linear->GetProb() << std::endl;


        c1->Update();
        c1->SaveAs("../../../out/plot.pdf");
    }
}

/**
 * @brief Used to add fit parameters to the xValues and yValues vectors, based on the peak (single or double)
 * @param config 
 * @param analyzer 
 * @param yValues 
 * @param erryValues 
 * @param outfile 
 */
static void processFitParameters(const Config& config, Analyzer& analyzer, std::vector<double>& yValues, std::vector<double>& erryValues, std::ofstream& outfile) {

    double p3 = analyzer.getFitParameter(3);
    double v3 = analyzer.getFitParameterError(3);

    if (config.ftype == Analyzer::F1) {
        yValues.push_back(p3);
        erryValues.push_back(v3);
        outfile << config.filename << "\t" << config.histname << "\t" << p3 << " +- " << v3 << "\t" << "-" << "\n";
    }
    else if (config.ftype == Analyzer::F2) {
        double p6 = analyzer.getFitParameter(6);
        double v6 = analyzer.getFitParameterError(6);
        yValues.push_back(p3);
        yValues.push_back(p6);
        erryValues.push_back(v3);
        erryValues.push_back(v6);
        outfile << config.filename << "\t" << config.histname << "\t" << p3 << " +- " << v3 << "\t" << p6 << " +- " << v6 << "\n";
    }
}

/**
 *@brief  used to run analysis through configuration vector
 *config vector should be in the order in which class variables are declared
 * 
 *@param configs A vector of Config objects
 *@param onlyOneElement A bool that says whether the analyzed file contains Caesium, Cobalt, or both
 */ 
void runAnalysis(const std::vector<Config>& configs, bool onlyOneElement) {
    
    std::vector<TCanvas*> canvases;
    std::ofstream outfile("../../../out/peak_energies.txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file: peak_energies.txt" << std::endl;
        return;
    }
    outfile << "Filename	                            Histogram	            p3      p6      p9\n";

    std::vector<double> xValues, errxValues;
    std::vector<double> yValues = { 661,1173.2,1332.5,2505.7 }; //sempre gli stessi, presi dal sito NNDC
    int configCount = 0;
    int analysisPerFile = 3;

    for (const auto& config : configs) {
        Analyzer analyzer(config.filename, config.histname, config.ftype);
        analyzer.setUpperLowerBound(config.chn_lower_bound, config.chn_upper_bound);
        analyzer.setFitParameters(config.p0, config.p1, config.p2, config.p3, config.p4, config.p5, config.p6, config.p7, config.p8, config.p9, config.p10);
        analyzer.setActivity(config.activity, config.err_activity);
        analyzer.plot();
        analyzer.pulser();
        analyzer.trapefficiency(config.m);
        analyzer.normefficiency();
        canvases.push_back(analyzer.getCanvas());
        analyzer.saveResults();
        configCount++;
        processFitParameters(config, analyzer, xValues, errxValues, outfile);
    }
    
    if (!onlyOneElement) {
        GraphPlotter plotter(yValues, 4);
        plotter.setFitFunction("linear", "[0] + [1] * x", 0, 2300);
        plotter.addData(xValues, errxValues);

        for (int i = 0; i < configCount / analysisPerFile; ++i) {
            plotter.plotAndFit(i);
            plotter.printResidues(i);
            plotter.saveResults(i);
        }
    }

    outfile.close();

}

