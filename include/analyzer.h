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

    enum FuncType {F1, F2};

    Analyzer(const std::string& filename, const std::string& histname, FuncType ftype);
    ~Analyzer();
    void setFitParameters(double p0, double p1, double p2, double p3, double p4, double p5, double p6, double p7);
    void setUpperLowerBound(int chn_low, int chn_up);
    void efficiency(int m);
    void plot();
    TCanvas* Analyzer::getCanvas() const;

private:
    std::string filename;
    std::string histname;
    TFile* inFile;
    TH1F* histogram;
    TF1* func;
    TCanvas* canvas;
    FuncType ftype;
    double p0, p1, p2, p3, p4, p5, p6, p7;
    int chn_lower_bound, chn_upper_bound;

};

#endif // ANALYZER_H
