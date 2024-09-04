#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include <string>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>

/**
*@class Analyzer
*/
class Analyzer {
public:
    //F1 single gaus, F2 double gaus, F3 triple gaus (unused), F4 single gausn
    enum FuncType {F1, F2, F3, F4};

    Analyzer(const std::string& filename, const std::string& histname, FuncType ftype);
    ~Analyzer();
    void setFitParameters(double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7, double p8, double p9, double p10);
    void setUpperLowerBound(int chn_low, int chn_up);
    void efficiency(int m);
    void plot();
    void saveResults();
    double getFitParameter(int index);
    double getFitParameterError(int index);
    TCanvas* Analyzer::getCanvas() const;

    //friend class Config;

private:
    std::string filename; /**< String containing the directory to the .root file */
    std::string histname; /**< String containing the directory to the histogram file */
    TFile* inFile; /**< Pointer to the ROOT file */
    TH1F* histogram; /**< Pointer to the histogram */
    TF1* func; /**< Pointer to the function to be fitted */
    TCanvas* canvas; /**< Pointer to the canvas */
    FuncType ftype; /**< Function type for fitting (gaussian or double gaussian) */
    double p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10; /**< Fit parameters */
    int chn_lower_bound, chn_upper_bound; /**< Upper and lower bound for fitting and peak area */
    double livetime, err_livetime;
    double activity, err_activity;
    double effic, err_effic;

};

#endif // ANALYZER_H
