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

int main(int argc, char* argv[]) {
    TApplication app("app", &argc, argv);

    display("welcome");

    // The order of parameters is: filename, histname, funcType, chn_lower_bound, chn_upper_bound, m, pulser_min, pulser_max, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10.
    std::vector<Config> configs1775 = {
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F4, 460, 650, 40, 2905, 2942, -0.45, 471, 92000, 550, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F5, 850, 1255, 40, 2905, 2942, -0.2, 471, 50000, 1000, 40, 50000, 1125, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F4, 1900, 2300, 40, 2905, 2942, -0.01, 30, 2000, 2100, 70},

        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F4, 455, 650, 40, 2825, 2869, -0.45, 600, 90000, 560, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F5, 875, 1250, 40, 2825, 2869, -0.2, 471, 32000, 1000, 40, 40000, 1150, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F4, 2000, 2300, 40, 2825, 2869, -0.01, 30, 2100, 2130, 70},

        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F4, 400, 575, 40, 2836, 2880, -0.45, 600, 120000, 485, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F5, 775, 1075, 40, 2836, 2880, -0.2, 471, 40000, 875, 40, 45000, 950, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F4, 1700, 2000, 40, 2836, 2880, -0.01, 30, 3500, 1840, 70},

        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F4, 450, 600, 40, 2910, 2950, -0.45, 1100, 350000, 525, 50},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F5, 840, 1145, 40, 2910, 2950, -0.2, 471, 80000, 925, 40, 120000, 1050, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F4, 1840, 2100, 40, 2910, 2950, -0.01, 30, 10500, 1970, 70},

        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F4, 425, 610, 40, 2742, 2782, -0.45, 471, 240000, 515, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F5, 825, 1150, 40, 2742, 2782, -0.2, 1100, 75000, 930, 50, 75000, 1040, 50},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F4, 1830, 2150, 40, 2742, 2782, -0.01, 30, 7500, 1985, 70},

        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F4, 490, 630, 40, 2815, 2850, -0.45, 500, 80000, 580, 40},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F5, 925, 1270, 40, 2815, 2850, -0.2, 471, 32000, 1040, 40, 45000, 1170, 60},
        {"../../../root files/run1775_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F4, 2050, 2350, 40, 2815, 2850, -0.01, 30, 7000, 2190, 70}

    };
    /*
    std::vector<Config> configs1776_F1 = {
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F1, 450, 650, 40, -0.5, 400, 0.009, 560, 40},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F1, 380, 585, 40, -0.5, 400, 0.01,  500, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F1, 410, 620, 40, -0.5, 400, 0.01,  520, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F1, 400, 630, 40, -0.5, 400, 0.01,  520, 60},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F1, 450, 685, 30, -0.5, 400, 0.01,  590, 50},
    };*/

    std::vector<Config> configs1776 = {
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F4, 450, 650, 250, 2880, 2930, -0.5, 400, 0.009, 560, 40},
        /*{"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F4, 450, 650, 2800, 2800, 2850, -0.5, 400, 0.009, 560, 40},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F4, 380, 585, 2820, 2820, 2880, -0.5, 400, 0.01,  500, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F4, 410, 620, 2890, 2890, 2930, -0.5, 400, 0.01,  520, 50},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F4, 400, 630, 2720, 2720, 2760, -0.5, 400, 0.01,  520, 60},
        {"../../../root files/run1776_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F4, 450, 685, 2810, 2810, 2850, -0.5, 400, 0.01,  590, 50},*/
    };

    std::vector<Config> configs1777_F5 = {
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F5, 500, 1600, 40, 2898, 2929, -0.05, 150, 22000, 1000, 50, 22000, 1140, 50},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F5, 500, 1650, 40, 2822, 2852, -0.05, 150, 22000, 1000, 50, 22000, 1110, 50},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F5, 500, 1600, 40, 2838, 2874, -0.05, 150, 22000, 895, 50, 22000, 980, 50},
        /*{"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F5, 800, 1154, 40, 2905, 2936, -0.05, 150, 22000, 925, 50, 22000, 1049, 50},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F5, 810, 1195, 40, 2737, 2770, -0.05, 150, 22000, 947, 50, 22000, 1050, 50},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F5, 920, 1300, 40, 2816, 2850, -0.05, 150, 22000, 1051, 50, 22000, 1181, 50}*/
    };

    std::vector<Config> configs1777_F3 = {
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F3, 500, 1600, 40, 2898, 2929, -0.001, 150, 22000, 1000, 50, 22000, 1140, 50, 1e-5},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F3, 500, 1650, 40, 2822, 2852, -0.003, 150, 22000, 1000, 50, 22000, 1110, 50, 1e-5},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F3, 400, 1600, 40, 2838, 2874, -0.1, 150, 22000, 895, 50, 22000, 980, 50, 1e-5},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F3, 300, 1654, 40, 2905, 2936, -0.1, 150, 22000, 925, 50, 22000, 1049, 50, 1e-5},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F3, 300, 1630, 40, 2737, 2770, -0.05, 150, 22000, 947, 50, 22000, 1050, 50, 1e-5},
        {"../../../root files/run1777_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F5, 500, 1600, 40, 2816, 2850, -0.05, 150, 22000, 1051, 50, 22000, 1181, 50, 1e-5}
    };

    std::vector<Config> configs1779 = {
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F5, 900, 1250, 40, 2908, 2942, -0.05, 150, 40000, 1015, 50, 40000, 1140, 50},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F5, 861, 1250, 40, 2833, 2866, -0.05, 150, 40000, 997, 50, 40000, 1120, 50},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F5, 765, 1120, 40, 2836, 2876, -0.05, 150, 40000, 890, 50, 40000, 985, 50},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F5, 815, 1150, 40, 2912, 2942, -0.05, 150, 40000, 923, 50, 40000, 1042, 50},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F5, 805, 1200, 40, 2746, 2778, -0.05, 150, 40000, 946, 50, 40000, 1044, 50},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F5, 900, 1300, 40, 2816, 2850, -0.05, 150, 40000, 1042, 50, 40000, 1172, 50}
    };

    std::vector<Config> configs1779_F3 = {
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F3, 500, 1650, 40, 2908, 2942, -0.05, 150, 40000, 1015, 50, 40000, 1140, 50, 4e-6},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F3, 501, 1650, 40, 2833, 2866, -0.05, 150, 40000, 997, 50, 40000, 1120, 50, 4e-5},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F3, 405, 1420, 40, 2836, 2876, -0.15, 150, 40000, 890, 50, 40000, 985, 50, 4e-5},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F3, 415, 1550, 40, 2912, 2942, -0.05, 150, 40000, 923, 50, 40000, 1042, 50, 4e-5},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F3, 405, 1600, 40, 2746, 2778, -0.05, 150, 40000, 946, 50, 40000, 1044, 50, 4e-5},
        {"../../../root files/run1779_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F3, 500, 1600, 40, 2816, 2850, -0.05, 150, 40000, 1042, 50, 40000, 1172, 50, 4e-5}/**/
    };

    std::vector<Config> configs1780 = { 
        { "../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_1", Analyzer::F4, 460, 650, 2910, 2910, 2940, -0.5, 250, 80000, 560, 50 },
        { "../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_2", Analyzer::F4, 440, 660, 2838, 2838, 2866, -0.5, 250, 80000, 550, 50 },
        { "../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_3", Analyzer::F4, 390, 580, 2840, 2840, 2874, -0.5, 250, 80000, 490, 50 },
        { "../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_4", Analyzer::F4, 410, 595, 2920, 2920, 2946, -0.5, 250, 80000, 510, 50 }, //fix parameters
        { "../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_5", Analyzer::F4, 400, 620, 2750, 2750, 2780, -0.5, 250, 80000, 510, 40 },
        { "../../../root files/run1780_coinc.root", "EnergyADC/h_EBGO_ADC_6", Analyzer::F4, 465, 660, 2820, 2820, 2844, -0.5, 250, 80000, 575, 50 },
    };


    //runAnalysis(configs1775, false); //total time unknown
    //runAnalysis(configs1776, true); //results are coherent
    runAnalysis(configs1777_F5, true); //results are not very coherent
    //runAnalysis(configs1777_F3, true);
    //runAnalysis(configs1778, true); //total time unknown
    //runAnalysis(configs1779, true); //results are coherent
    //runAnalysis(configs1779_F3, true);
    //runAnalysis(configs1780, true); //results are coherent

    app.Run();
    return 0;
}