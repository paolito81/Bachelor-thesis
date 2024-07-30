#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <utils.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <filesystem>
#include <cmath>

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


/**
 * @brief  used to run analysis through configuration vector
 * config vector should be in the order in which class variables are declared
 * 
 * @param configs A vector of Config objects
 */ 

void runAnalysis(const std::vector<Config>& configs) {
    std::vector<TCanvas*> canvases;
    for (const auto& config : configs) {
        Analyzer analyzer(config.filename, config.histname, config.ftype);
        analyzer.setUpperLowerBound(config.chn_lower_bound, config.chn_upper_bound);
        analyzer.setFitParameters(config.p0, config.p1, config.p2, config.p3, config.p4, config.p5, config.p6, config.p7);
        analyzer.efficiency(config.m);
        analyzer.plot();
        canvases.push_back(analyzer.getCanvas());
        analyzer.saveResults();
    }

}