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

const std::filesystem::path welcomeFilePath{ "../../../welcome.txt" };

void isTFileOpen(TFile* inFile) {
	if (!inFile || inFile->IsZombie())
	{
		std::cerr << "Failed to open file: " << inFile->GetName() << std::endl;
		exit(1);
	}
}

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

double trap_area(TH1F* histogram, int chn_1, int chn_2, int m) {
    double area = (histogram->GetBinContent(histogram->FindBin(chn_1) - 1 - m) + histogram->GetBinContent(histogram->FindBin(chn_2) + 1 + m)) * (chn_2 - chn_1) / 2;
    return area;
}

// used to calculate the uncertainty of the calculated peak area

double var_peak(double area, double trap, int n, int m) {
    double variance = std::sqrt(area + trap*(1 + n/(2*m)));
    return variance;
}

void MakeGraphErrors(int configCount, int elementsPerVector, std::vector<double> yValues, std::vector<double> erryValues) {
    std::vector<double> xValues = { 661,1173.2,1332.5,2505.7 };
    std::vector<std::vector<double>> sep_yValues;
    std::vector<std::vector<double>> sep_erryValues;
    for (int i = 0; i < configCount / 3; ++i) {
        std::vector<double> yTemp;
        std::vector<double> yerrTemp;
        for (int j = 0; j < elementsPerVector; ++j) {
            yTemp.push_back(yValues[i * elementsPerVector + j]);
            yerrTemp.push_back(erryValues[i * elementsPerVector + j]);
        }
        sep_yValues.push_back(yTemp);
        sep_erryValues.push_back(yerrTemp);
    }

    for (int i = 0; i < configCount / 3; ++i) {
        TGraphErrors* graph = new TGraphErrors(xValues.size(), sep_yValues[i].data(), xValues.data(), sep_erryValues[i].data(), nullptr);
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
        linear->GetProb();


        c1->Update();
        c1->SaveAs("../../../out/plot.pdf");
    }
}
/*
void processFitParameters(const Config& config, Analyzer& analyzer, std::vector<double>& yValues, std::vector<double>& erryValues, std::ofstream& outfile) {

}
*/
/**
 * @brief  used to run analysis through configuration vector
 * config vector should be in the order in which class variables are declared
 * 
 * @param configs A vector of Config objects
 */ 

void runAnalysis(const std::vector<Config>& configs) {
    
    std::vector<TCanvas*> canvases;
    std::ofstream outfile("../../../out/peak_energies.txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file: peak_energies.txt" << std::endl;
        return;
    }
    outfile << "Filename	                            Histogram	            p3      p6      p9\n";

    std::vector<double> yValues, erryValues;
    std::vector<double> xValues = { 661,1173.2,1332.5,2158.6 }; //boh
    int configCount = 0;

    for (const auto& config : configs) {
        Analyzer analyzer(config.filename, config.histname, config.ftype);
        analyzer.setUpperLowerBound(config.chn_lower_bound, config.chn_upper_bound);
        analyzer.setFitParameters(config.p0, config.p1, config.p2, config.p3, config.p4, config.p5, config.p6, config.p7, config.p8, config.p9, config.p10);
        //analyzer.efficiency(config.m);
        analyzer.plot();
        canvases.push_back(analyzer.getCanvas());
        analyzer.saveResults();
        configCount++;

        if (config.ftype == Analyzer::F1) {
            double p3 = analyzer.getFitParameter(3);
            double v3 = analyzer.getFitParameterError(3);
            yValues.push_back(p3);
            erryValues.push_back(v3);
            outfile << config.filename << "\t" << config.histname << "\t" << p3 << " +- " << v3 << "\t" << "-" << "\n";
        }
        else if (config.ftype == Analyzer::F2) {
            double p3 = analyzer.getFitParameter(3);
            double p6 = analyzer.getFitParameter(6);
            double v3 = analyzer.getFitParameterError(3);
            double v6 = analyzer.getFitParameterError(6);
            yValues.push_back(p3);
            yValues.push_back(p6);
            erryValues.push_back(v3);
            erryValues.push_back(v6);
            outfile << config.filename << "\t" << config.histname << "\t" << p3 << " +- " << v3 << "\t" << p6 << " +- " << v6 << "\n";
        }
    }
    
    MakeGraphErrors(configCount, 4, yValues, erryValues);

    outfile.close();

}

