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
    /*
    TF1* fourpoly = new TF1("fourpoly", "[0] + [1]*x + [2]*x^2 + [3]*x^3 + [4]*x^4", 0, 2300);
    fourpoly->SetParameters(0, 0, 0, 0, 0);
    */
    TF1* linear = new TF1("linear", "[0] + [1]*x", 0, 2220);
    linear->SetParameters(0, 1);

    graph->SetTitle("grapho");
    graph->GetYaxis()->SetTitle("Energy [keV]");
    graph->GetXaxis()->SetTitle("Channels [CHN]");
    graph->SetMarkerStyle(21);
    graph->Draw("AP");

    graph->Fit("linear", "R");
    //fourpoly->GetProb();
    std::cout << "P-value: " << linear->GetProb() << std::endl;

    std::string pdfName = "../../../out/plot" + std::to_string(i) + ".pdf";

    c1->Update();
    c1->SaveAs(pdfName.c_str());
}