#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <vector>
#include <string>
#include <graphplotter.h>
#include <TAxis.h>

/**
* @brief The constructor for the GraphPlotter object
*
*/

GraphPlotter::GraphPlotter(const std::vector<double>& yValues, int elementsPerVector)
    : yValues(yValues), elementsPerVector(elementsPerVector) {}

/**
* @brief adds selected data in arrays to the class member sep_xValues and sep_errxValues
* 
* @param xValues the values that one wishes to add on the x axis
* @param errxValues the errors of the xValues
*/

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

/**
* @brief A function to plot and fit a function to the xValues and yValues
* 
* @param i The index of the separated values vector
*/

void GraphPlotter::plotAndFit(int index) {
    
    TGraphErrors* graph = new TGraphErrors(yValues.size(), sep_xValues[index].data(), yValues.data(), sep_errxValues[index].data(), nullptr);
    std::string canvasName = "c" + std::to_string(index);
    TCanvas* c1 = new TCanvas(canvasName.c_str(), "meanplots", 800, 600);

    func->SetParameters(0, 1);

    graph->SetTitle("Energy calibration");
    graph->GetYaxis()->SetTitle("Energy [keV]");
    graph->GetXaxis()->SetTitle("Channels [CHN]");
    graph->SetMarkerStyle(21);
    graph->Draw("AP");


    std::cout << "\n\n\n\n" << std::endl;
    graph->Fit(func->GetName(), "R");
    std::cout << "P-value: " << func->GetProb() << std::endl;

    std::string pdfName = "../../../out/plot" + std::to_string(index) + ".pdf";

    c1->Update();
    c1->SaveAs(pdfName.c_str());
}

/**
* @brief A function to set the function to be fit to the data
* 
* @param funcName A const char* to the function name
* @param formula A const char* to the funcion formula
* @param xmin The minimum value of the function
* @param xmax The maximum value of the function
*/

void GraphPlotter::setFitFunction(const std::string& funcName, const std::string& formula, double xmin, double xmax) {
    //if (func != nullptr) {
    //    delete func; // clean up previous function if it exists
    //}
    func = new TF1(funcName.c_str(), formula.c_str(), xmin, xmax);
}

void GraphPlotter::printResidues(int index) {
        std::vector<double>& xTemp = sep_xValues[index];
        std::cout << "============ RESIDUES =============\n";
        std::cout << "Residues for this plot: ";
        for (size_t i = 0; i < yValues.size(); ++i) {
            std::cout << (func->Eval(xTemp[i]) - yValues[i]) << "\t";
        }
        std:: cout << "\n\n";
}