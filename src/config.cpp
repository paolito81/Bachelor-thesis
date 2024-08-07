#include <analyzer.h>
#include <config.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include "run1776.h"
#include "utils.h"
#include <fstream>
#include <filesystem>

void Config::printMeanValues(std::ofstream outfile, Analyzer analyzer) {
    
    if (ftype == Analyzer::F1) {
        double p3 = analyzer.getFitParameter(3);
        double v3 = analyzer.getFitParameterError(3);
        outfile << filename << "\t" << histname << "\t" << p3 << " +- " << v3 << "\t" << "-" << "\n";
    }
    else if (ftype == Analyzer::F2) {
        double p3 = analyzer.getFitParameter(3);
        double p6 = analyzer.getFitParameter(6);
        double v3 = analyzer.getFitParameterError(3);
        double v6 = analyzer.getFitParameterError(6);
        outfile << filename << "\t" << histname << "\t" << p3 << " +- " << v3 << "\t" << p6 << " +- " << v6 << "\n";
    }
}