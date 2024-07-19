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

    std::vector<Config> configs = {
        /*{"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F1, 450, 650, 40, -0.5, 400, 0.009, 560, 40},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F1, 380, 585, 40, -0.5, 400, 0.01,  500, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F1, 410, 620, 40, -0.5, 400, 0.01,  520, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F1, 400, 630, 40, -0.5, 400, 0.01,  520, 60},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F1, 450, 685, 30, -0.5, 400, 0.01,  590, 50},*/
        
        /*{"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F2, 800, 1350, 40, -0.06, 130, 1000, 50, 0.01, 1175, 50, 0.01},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F2, 800, 1350, 40, -0.06, 130, 1000, 50, 0.01, 1175, 50, 0.01}*/

        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F1, 450, 650, 40, -0.12, 150, 800, 550, 50},
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F1, 440, 660, 40, -0.12, 150, 820, 550, 50},
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F1, 390, 580, 40, -0.12, 150, 850, 500, 50},
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F1, 410, 595, 40, -0.50, 300, 1000, 510, 50}, //fix parameters
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F1, 400, 620, 40, -0.12, 150, 800, 510, 40},
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F1, 465, 660, 40, -0.12, 150, 800, 590, 50},
    };

    runAnalysis(configs);

    app.Run();
    return 0;
}