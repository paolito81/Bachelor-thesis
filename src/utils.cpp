#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <utils.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <filesystem>

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

static double trap_area(TH1F* histogram, int chn_1, int chn_2) {
    double area = (histogram->GetBinContent(histogram->FindBin(chn_1) - 1) + histogram->GetBinContent(histogram->FindBin(chn_2) + 1)) * (chn_2 - chn_1) / 2;
    return area;
}

// used to calculate the uncertainty of the calculated peak area

static double var_peak(TH1F* histogram, int chn_1, int chn_2) {
    int variance;
    return variance;
}
