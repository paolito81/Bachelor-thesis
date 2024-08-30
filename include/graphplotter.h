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
    void addData(const std::vector<double>& xValues, const std::vector<double>& errxValues);
    void plotAndFit(int index);

private:
    std::vector<double> yValues;
    int elementsPerVector;
    std::vector<std::vector<double>> sep_xValues;
    std::vector<std::vector<double>> sep_errxValues;
};

#endif // GRAPHPLOTTER_H