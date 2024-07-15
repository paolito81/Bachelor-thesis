#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include <string>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>

class Analyzer {
public:
    Analyzer(const std::string& filename, const std::string& histname);
    ~Analyzer();
    void setFitParameters(double p0, double p1, double p2, double p3, double p4);
    void setUpperLowerBound(int chn_low, int chn_up);
    void efficiency(int m);
    void plot();

private:
    std::string filename;
    std::string histname;
    TFile* inFile;
    TH1F* histogram;
    TF1* func;
    TCanvas* canvas;
    double p0, p1, p2, p3, p4;
    int chn_lower_bound, chn_upper_bound;

};

#endif // ANALYZER_H
