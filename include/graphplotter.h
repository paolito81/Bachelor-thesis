#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <vector>
#include <string>

class GraphPlotter {
public:
    GraphPlotter(const std::vector<double>& xValues, int elementsPerVector);
    void addData(const std::vector<double>& yValues, const std::vector<double>& erryValues);
    void plotAndFit(int index);

private:
    std::vector<double> xValues;
    int elementsPerVector;
    std::vector<std::vector<double>> sep_yValues;
    std::vector<std::vector<double>> sep_erryValues;
};

#endif // GRAPHPLOTTER_H