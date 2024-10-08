#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <vector>
#include <string>

class GraphPlotter {
public:
    GraphPlotter(const std::vector<double>& yValues, int elementsPerVector);
    void setFitFunction(const std::string& funcName, const std::string& formula, double xmin, double xmax);
    void setAndPrintResidues(int index);
    void addData(const std::vector<double>& xValues, const std::vector<double>& errxValues);
    void plotAndFit(int index);
    void saveResults(int index);
    void plotResidues(int index);

private:
    std::vector<double> yValues;
    int elementsPerVector;
    std::vector<std::vector<double>> sep_xValues;
    std::vector<std::vector<double>> sep_errxValues;
    std::vector<double> residues;
    TF1* func;
};

#endif // GRAPHPLOTTER_H