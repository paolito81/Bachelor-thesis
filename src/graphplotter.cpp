#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <vector>
#include <string>
#include <graphplotter.h>
#include <TAxis.h>

GraphPlotter::GraphPlotter(const std::vector<double>& yValues, int elementsPerVector)
    : yValues(yValues), elementsPerVector(elementsPerVector) {}

void GraphPlotter::addData(const std::vector<double>& xValues, const std::vector<double>& errxValues) {
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
    for (size_t i = 0; i < xValues.size() / elementsPerVector; ++i) {
        std::vector<double> xTemp(xValues.begin() + i * elementsPerVector, xValues.begin() + (i + 1) * elementsPerVector);
        std::vector<double> xerrTemp(errxValues.begin() + i * elementsPerVector, errxValues.begin() + (i + 1) * elementsPerVector);
        sep_xValues.push_back(xTemp);
        sep_errxValues.push_back(xerrTemp);
    }
}

void GraphPlotter::plotAndFit(int i) {
    
    TGraphErrors* graph = new TGraphErrors(yValues.size(), sep_xValues[i].data(), yValues.data(), sep_errxValues[i].data(), nullptr);
    std::string canvasName = "c" + std::to_string(i);
    TCanvas* c1 = new TCanvas(canvasName.c_str(), "meanplots", 800, 600);

    func->SetParameters(0, 1);

    graph->SetTitle("Energy calibration");
    graph->GetYaxis()->SetTitle("Energy [keV]");
    graph->GetXaxis()->SetTitle("Channels [CHN]");
    graph->SetMarkerStyle(21);
    graph->Draw("AP");

    graph->Fit(func->GetName(), "R");
    std::cout << "P-value: " << func->GetProb() << std::endl;

    std::string pdfName = "../../../out/plot" + std::to_string(i) + ".pdf";

    c1->Update();
    c1->SaveAs(pdfName.c_str());
}

void GraphPlotter::setFitFunction(const std::string& funcName, const std::string& formula, double xmin, double xmax) {
    if (func != nullptr) {
        delete func; // Clean up previous function if it exists
    }
    func = new TF1(funcName.c_str(), formula.c_str(), xmin, xmax);
}