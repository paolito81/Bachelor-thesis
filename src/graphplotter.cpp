#include <TGraphErrors.h>
#include <TGraph.h>
#include <TF1.h>
#include <TCanvas.h>
#include <vector>
#include <string>
#include <graphplotter.h>
#include <TAxis.h>
#include <fstream>
#include <utils.h>
#include <TLine.h>

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
*@brief A function to plot and fit a function to the xValues and yValues
* 
*@param i The index of the separated values vector
*/
void GraphPlotter::plotAndFit(int index) {
    
    TGraphErrors* graph = new TGraphErrors(yValues.size(), sep_xValues[index].data(), yValues.data(), sep_errxValues[index].data(), nullptr);
    std::string canvasName = "c" + std::to_string(index);
    TCanvas* c1 = new TCanvas(canvasName.c_str(), "meanplots", 800, 600);
    c1->SetLeftMargin(0.15);
    func->SetParameters(0, 1);

    double fontsize = 0.053;

    graph->GetXaxis()->SetLabelSize(fontsize);  // Set X-axis label font size
    graph->GetYaxis()->SetLabelSize(fontsize);  // Set Y-axis label font size
    graph->GetXaxis()->SetTitleSize(fontsize);  // Set X-axis title font size
    graph->GetYaxis()->SetTitleSize(fontsize);  // Set Y-axis title font size
    graph->SetTitle("BGO 4 - Energy = a + b*CHN");
    graph->GetYaxis()->SetTitle("Energy [keV]");
    graph->GetXaxis()->SetTitle("Channels [CHN]");
    graph->SetMarkerStyle(21);
    graph->Draw("AP");


    std::cout << "\n\n\n\n" << std::endl;
    graph->Fit(func->GetName(), "R");
    std::cout << "P-value: " << func->GetProb() << std::endl;

    std::string pdfName = "../../../out/residues plots/plot" + std::to_string(index) + ".pdf";

    c1->Update();
    c1->SaveAs(pdfName.c_str());
}

/**
*@brief A function to set the function to be fit to the data
* 
*@param funcName A const char* to the function name
*@param formula A const char* to the funcion formula
*@param xmin The minimum value of the function
*@param xmax The maximum value of the function
*/
void GraphPlotter::setFitFunction(const std::string& funcName, const std::string& formula, double xmin, double xmax) {
    //if (func != nullptr) {
    //    delete func; // clean up previous function if it exists
    //}
    func = new TF1(funcName.c_str(), formula.c_str(), xmin, xmax);
}

// add a plotting method
void GraphPlotter::setAndPrintResidues(int index) {
        std::vector<double>& xTemp = sep_xValues[index];
        std::cout << "============ RESIDUES =============\n";
        std::cout << "Residues for this plot: ";
        for (size_t i = 0; i < yValues.size(); ++i) {
            double residue = (func->Eval(xTemp[i]) - yValues[i]);
            std::cout << residue << "\t";
            residues.push_back(residue);
        }
        std:: cout << "\n\n";
}

/**
 * @brief Function to save the results in a .txt file of the GraphPlotter, used for residues
 * @param index 
 */
void GraphPlotter::saveResults(int index) {
    
    std::string outputFilePath = "../../../out/residues/histogram_" + std::to_string(index) + ".txt";

    std::ofstream outFile(outputFilePath);
    if (!outFile.is_open()) {
        std::cerr << "Unable to open output file: " << outputFilePath << std::endl;
        return;
    }

    if (func) {
        outFile << "******************************************\n";
        outFile << "Fit Results:\n\n";
        for (int i = 0; i < func->GetNpar(); ++i) {
            outFile << func->GetParName(i) << " : " << func->GetParameter(i) << " +/- " << func->GetParError(i) << "\n";
        }
    }

    outFile << "Fit p-value: " << func->GetProb() << "\n";

    outFile << "Residues: [\t";
    for (size_t i = index * 4; i < residues.size(); ++i) {
        outFile << residues[i] << "\t";
    }
    outFile << "]\n";

    //outFile << "Efficiency value: " << effic << " +- " << err_effic << "\n";

    outFile << "******************************************";
    outFile.close();

    std::cout << "Results saved to " << outputFilePath << std::endl;
    std::cout << "\n\n";
}

/**
 * @brief Function to plot the residues in a graph, for displaying purposes
 * @param index 
 */
void GraphPlotter::plotResidues(int index) {
    std::vector<double> x_counts = { 661,1173.2,1332.5,2505.7 };
    std::string canvasName = "residueCanvas" + std::to_string(index);
    std::vector<double> tempRes;

    for (size_t i = index * 4; i < residues.size(); ++i) {
        tempRes.push_back(residues[i]);
    }

    TCanvas* residueCanvas = new TCanvas(canvasName.c_str(), "Residues", 800, 600);
    TGraph* residueGraph = new TGraph(tempRes.size(), x_counts.data(), tempRes.data());
    residueCanvas->SetLeftMargin(0.15);
    double fontsize = 0.053;

    residueGraph->GetXaxis()->SetLabelSize(fontsize);  // Set X-axis label font size
    residueGraph->GetYaxis()->SetLabelSize(fontsize);  // Set Y-axis label font size
    residueGraph->GetXaxis()->SetTitleSize(fontsize);  // Set X-axis title font size
    residueGraph->GetYaxis()->SetTitleSize(fontsize);  // Set Y-axis title font size

    TLine* zeroline = new TLine(495, 0, 2700, 0);
    zeroline->SetLineColor(kBlue);

    std::string graphName = "Residues: BGO " + std::to_string(index + 1);

    residueGraph->SetTitle(graphName.c_str());
    residueGraph->GetYaxis()->SetTitle("Residue [keV]");
    residueGraph->GetXaxis()->SetTitle("Energy [keV]");
    residueGraph->SetMarkerStyle(21);
    residueGraph->SetMarkerSize(1);

    residueGraph->Draw("AP");
    zeroline->Draw();
    residueCanvas->Update();


    std::string outputPath = "../../../out/residues graphs/" + canvasName + ".pdf";

    residueCanvas->SaveAs(outputPath.c_str());
 }