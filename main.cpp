#include <iostream>
#include <utils.h>
#include <string>
#include <TF1.h>
#include <TH1F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <run1776.h>
#include <TApplication.h>
#include "analyzer.h"
#include <vector>

int main (int argc, char* argv[]) {
    TApplication app("app", &argc, argv);

    display("welcome");

    //run1776();
    /*
    Analyzer analyzer("../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_2");
    analyzer.setUpperLowerBound(450, 650);
    analyzer.setFitParameters(1 / 2, 400, 1, 550, 44);
    analyzer.efficiency(13);
    analyzer.plot();
    */

    std::vector<Config> configs = {
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_2", 1 / 2, 400, 1, 550, 44, 450, 650, 10}
    };

    runAnalysis(configs);

    app.Run();
    return 0;
}