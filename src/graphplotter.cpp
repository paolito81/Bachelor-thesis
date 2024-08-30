#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <vector>
#include <string>
#include <graphplotter.h>
#include <TAxis.h>

GraphPlotter::GraphPlotter(const std::vector<double>& xValues, int elementsPerVector)
    : xValues(xValues), elementsPerVector(elementsPerVector) {}

void GraphPlotter::addData(const std::vector<double>& yValues, const std::vector<double>& erryValues) {
    /*for (int i = 0; i < configCount / 3; ++i) {
        std::vector<double> yTemp;
        std::vector<double> yerrTemp;
        for (int j = 0; j < elementsPerVector; ++j) {
            yTemp.push_back(yValues[i * elementsPerVector + j]);
            yerrTemp.push_back(erryValues[i * elementsPerVector + j]);
        }
        sep_yValues.push_back(yTemp);
        sep_erryValues.push_back(yerrTemp);
    }*/
    for (size_t i = 0; i < yValues.size() / elementsPerVector; ++i) {
        std::vector<double> yTemp(yValues.begin() + i * elementsPerVector, yValues.begin() + (i + 1) * elementsPerVector);
        std::vector<double> yerrTemp(erryValues.begin() + i * elementsPerVector, erryValues.begin() + (i + 1) * elementsPerVector);
        sep_yValues.push_back(yTemp);
        sep_erryValues.push_back(yerrTemp);
    }
}

void GraphPlotter::plotAndFit(int i) {
    
    TGraphErrors* graph = new TGraphErrors(xValues.size(), sep_yValues[i].data(), xValues.data(), sep_erryValues[i].data(), nullptr);
    std::string canvasName = "c" + std::to_string(i);
    TCanvas* c1 = new TCanvas(canvasName.c_str(), "meanplots", 800, 600);
    /*
    TF1* fourpoly = new TF1("fourpoly", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4", 0, 2300);
    fourpoly->SetParameters(0, 0, 0, 0, 0);
    */
    TF1* linear = new TF1("fourpoly", "[0] + [1]*x", 0, 2300);
    linear->SetParameters(0, 1);

    graph->SetTitle("grapho");
    graph->GetYaxis()->SetTitle("Energy [keV]");
    graph->GetXaxis()->SetTitle("Channels [CHN]");
    graph->SetMarkerStyle(21);
    graph->Draw("AP");

    graph->Fit("fourpoly", "R");
    //fourpoly->GetProb();
    linear->GetProb();


    c1->Update();
    c1->SaveAs("../../../out/plot.pdf");
}