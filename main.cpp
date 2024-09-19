#include <iostream>
#include <utils.h>
#include <string>
#include <TF1.h>
#include <TH1F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TApplication.h>
#include "analyzer.h"
#include <vector>

int main (int argc, char* argv[]) {
    TApplication app("app", &argc, argv);

    display("welcome");

    // The order of parameters is: filename, histname, funcType, chn_lower_bound, chn_upper_bound, m, pulser_min, pulser_max, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10.
    std::vector<Config> configs1775 = {
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F1, 460, 650, 40, -0.45, 471, 2300, 550, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F2, 850, 1255, 40, -0.2, 471, 800, 1000, 40, 800, 1125, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F1, 1900, 2300, 40, -0.01, 30, 30, 2100, 70},
        
        /*{"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F1, 455, 650, 40, -0.45, 600, 2300, 560, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F2, 875, 1250, 40, -0.2, 471, 800, 1000, 40, 800, 1150, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F1, 2000, 2300, 40, -0.01, 30, 30, 2130, 70},

        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F1, 400, 575, 40, -0.45, 600, 3000, 485, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F2, 775, 1075, 40, -0.2, 471, 1000, 875, 40, 800, 950, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F1, 1700, 2000, 40, -0.01, 30, 50, 1840, 70},
        
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F1, 450, 600, 40, -0.45, 1100, 7000, 525, 50},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F2, 840, 1145, 40, -0.2, 471, 1800, 925, 40, 1800, 1050, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F1, 1840, 2100, 40, -0.01, 30, 150, 1970, 70},
        
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F1, 425, 610, 40, -0.45, 471, 6000, 515, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F2, 825, 1150, 40, -0.2, 1100, 1500, 930, 50, 1500, 1040, 50},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F1, 1830, 2150, 40, -0.01, 30, 150, 1985, 70},
        
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F1, 490, 630, 40, -0.45, 500, 2300, 580, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F2, 925, 1270, 40, -0.2, 471, 800, 1040, 40, 800, 1170, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F1, 2075, 2330, 40, -0.1, 150, 50, 2190, 70},*/

       
        
        

        /*{"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F1, 450, 650, 40, -0.12, 150, 800, 550, 50},
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F1, 440, 660, 40, -0.12, 150, 820, 550, 50},
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F1, 390, 580, 40, -0.12, 150, 850, 500, 50},
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F1, 410, 595, 40, -0.50, 300, 1000, 510, 50}, //fix parameters
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F1, 400, 620, 40, -0.12, 150, 800, 510, 40},
        {"../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F1, 465, 660, 40, -0.12, 150, 800, 590, 50},*/

    };

    std::vector<Config> configs1776_F1 = {
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F1, 450, 650, 40, -0.5, 400, 0.009, 560, 40},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F1, 380, 585, 40, -0.5, 400, 0.01,  500, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F1, 410, 620, 40, -0.5, 400, 0.01,  520, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F1, 400, 630, 40, -0.5, 400, 0.01,  520, 60},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F1, 450, 685, 30, -0.5, 400, 0.01,  590, 50},
    };

    std::vector<Config> configs1776_F4 = {
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F4, 450, 650, 70, 2880, 2930, -0.5, 400, 0.009, 560, 40},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F4, 450, 650, 70, 2800, 2850, -0.5, 400, 0.009, 560, 40},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F4, 380, 585, 70, 2820, 2880, -0.5, 400, 0.01,  500, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F4, 410, 620, 70, 2890, 2930, -0.5, 400, 0.01,  520, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F4, 400, 630, 70, 2720, 2760, -0.5, 400, 0.01,  520, 60},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F4, 450, 685, 70, 2810, 2850, -0.5, 400, 0.01,  590, 50},
    };

    std::vector<Config> configs1777_F5 = {
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F5, 800, 1350, 40, -0.06, 130, 0.01, 1000, 50, 0.01, 1175, 50},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F5, 800, 1350, 40, -0.06, 130, 0.01, 1000, 50, 0.01, 1175, 50}
    };

    std::vector<Config> configs1778_F5 = {
        {"../../../root files/run1778_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F5, 900, 1270, 40, -5, 6000,  20000, 1000, 50, 25000, 1130, 50},
        {"../../../root files/run1778_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F5, 900, 1200, 40, -5, 5000,  20000, 1000, 50, 25000, 1130, 50}
    };

    //runAnalysis(configs1775, false);
    runAnalysis(configs1776_F4, true);
    //runAnalysis(configs1777_F5, true);

    //runAnalysis(configs1778_F5, true);

    app.Run();
    return 0;
}