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
    //F1 single gaus, F2 double gaus, F3 triple gaus (unused), F4 single gausn, F5 double gausn
    enum FuncType {F1, F2, F3, F4, F5};

    Analyzer(const std::string& filename, const std::string& histname, FuncType ftype);
    ~Analyzer();
    void setFitParameters(double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7, double p8, double p9, double p10);
    void setUpperLowerBound(int chn_low, int chn_up);
    void setPeakUpperLower(int peakl, int peaku);
    void setActivity();
    void setTotalTime();
    void setRes();
    void printActivity() const;
    void trapefficiency(int m);
    void normefficiency();
    void trapefficiency_redux(int m);
    void printRefResolution();
    void plot();
    void saveResults();
    void pulser(int pulser_min, int pulser_max);
    void ZTestEfficiencies() const;
    double getFitParameter(int index);
    double getFitParameterError(int index);
    double getRefResolution();
    TCanvas* getCanvas() const;

protected:
    std::string filename; /**< String containing the directory to the .root file */
    std::string histname; /**< String containing the directory to the histogram file */
    TFile* inFile; /**< Pointer to the ROOT file */
    TH1F* histogram; /**< Pointer to the histogram */
    TF1* func; /**< Pointer to the function to be fitted */
    TCanvas* canvas; /**< Pointer to the canvas */
    FuncType ftype; /**< Function type for fitting (gaussian or double gaussian) */
    TLegend* legend; /** Legend for the canvas */
    double p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10; /**< Fit parameters */
    int chn_lower_bound, chn_upper_bound;
    int peak_lower, peak_upper;
    double activity, err_activity; /** The source's activity, with error */
    double pulser_integral; /** The integral over the pulser histogram (CHN0), used for livetime calculation */
    double total_time, time_perc, err_total_time;
    double efficiency1, efficiency2, err_efficiency1, err_efficiency2;
    double trap_efficiency, err_trap_efficiency;
    double resolution, err_resolution;
    double res1, err_res1, res2, err_res2;
};

#endif // ANALYZER_H
