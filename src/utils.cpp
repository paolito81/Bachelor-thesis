#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <iostream>
#include <utils.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <filesystem>
#include <cmath>
#include <config.h>
#include <TGraphErrors.h>
#include <graphplotter.h>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <regex>
#include <simulation.h>

const std::filesystem::path welcomeFilePath{ "../../../welcome.txt" };

//Used to check if TFile is open
void isTFileOpen(TFile* inFile) {
	if (!inFile || inFile->IsZombie())
	{
		std::cerr << "Failed to open file: " << inFile->GetName() << std::endl;
		exit(1);
	}
}

//Used to display .txt file on the terminal
void display(std::string filename) {
    std::string displaystr;
    std::ifstream inFile;

    inFile.open(welcomeFilePath);
    if (!inFile.is_open()) {
        std::cout << "Unable to open file";
        //throw std::runtime_error("Error opening welcome.txt");
        exit(1);
    }

    while (getline(inFile, displaystr)) {
        std::cout << displaystr << std::endl;
    }

    inFile.close();
}

// used to calculate background area under energy peak
static double trap_area(TH1F* histogram, int chn_1, int chn_2, int m) {
    double area = (histogram->GetBinContent(histogram->FindBin(chn_1) - 1 - m) + histogram->GetBinContent(histogram->FindBin(chn_2) + 1 + m)) * (chn_2 - chn_1) / 2;
    return area;
}

// used to calculate the uncertainty of the calculated peak area
static double var_peak(double area, double trap, int n, int m) {
    double variance = std::sqrt(area + trap*(1 + n/(2*m)));
    return variance;
}

//UNUSED
static void MakeGraphErrors(int configCount, int elementsPerVector, std::vector<double>& xValues, std::vector<double>& errxValues) {
    std::vector<double> yValues = { 661,1173.2,1332.5,2505.7 };
    std::vector<std::vector<double>> sep_xValues;
    std::vector<std::vector<double>> sep_errxValues;
    
    for (int i = 0; i < configCount / 3; ++i) {
        std::vector<double> xTemp;
        std::vector<double> xerrTemp;
        for (int j = 0; j < elementsPerVector; ++j) {
            xTemp.push_back(xValues[i * elementsPerVector + j]);
            xerrTemp.push_back(errxValues[i * elementsPerVector + j]);
        }
        sep_xValues.push_back(xTemp);
        sep_errxValues.push_back(xerrTemp);
    }

    for (int i = 0; i < configCount / 3; ++i) {
        TGraphErrors* graph = new TGraphErrors(xValues.size(), sep_xValues[i].data(), yValues.data(), sep_errxValues[i].data(), nullptr);
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
        std::cout << "P-value: " << linear->GetProb() << std::endl;


        c1->Update();
        c1->SaveAs("../../../out/plot.pdf");
    }
}

/**
 * @brief Used to add fit parameters to the xValues and yValues vectors, based on the peak (single or double)
 * @param config 
 * @param analyzer 
 * @param yValues 
 * @param erryValues 
 * @param outfile 
 */
static void processFitParameters(const Config& config, Analyzer& analyzer, std::vector<double>& yValues, std::vector<double>& erryValues, std::ofstream& outfile) {

    double p3 = analyzer.getFitParameter(3);
    double v3 = analyzer.getFitParameterError(3);

    if (config.ftype == Analyzer::F1 || config.ftype == Analyzer::F4) {
        yValues.push_back(p3);
        erryValues.push_back(v3);
        outfile << config.filename << "\t" << config.histname << "\t" << p3 << " +- " << v3 << "\t" << "-" << "\n";
    }
    else if (config.ftype == Analyzer::F2 || config.ftype == Analyzer::F5 || config.ftype == Analyzer::F3) {
        double p6 = analyzer.getFitParameter(6);
        double v6 = analyzer.getFitParameterError(6);
        yValues.push_back(p3);
        yValues.push_back(p6);
        erryValues.push_back(v3);
        erryValues.push_back(v6);
        outfile << config.filename << "\t" << config.histname << "\t" << p3 << " +- " << v3 << "\t" << p6 << " +- " << v6 << "\n";
    }
}

/**
 *@brief  used to run analysis through configuration vector.
 *config vector should be in the order in which class variables are declared. The order of parameters is: filename, histname, funcType, chn_lower_bound, chn_upper_bound, m, pulser_min, pulser_max, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10.
 * 
 *@param configs A vector of Config objects
 *@param onlyOneElement A bool that says whether the analyzed file contains Caesium, Cobalt, or both
 */ 
void runAnalysis(const std::vector<Config>& configs, bool onlyOneElement) {

    std::vector<TCanvas*> canvases;
    std::ofstream outfile("../../../out/peak_energies.txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file: peak_energies.txt" << std::endl;
        return;
    }
    outfile << "Filename	                            Histogram	            p3      p6      p9\n";

    std::vector<double> xValues, errxValues;
    std::vector<double> yValues = { 661,1173.2,1332.5,2505.7 }; //sempre gli stessi, presi dal sito NNDC
    int configCount = 0;
    int analysisPerFile = 3;
    std::vector<double> yValues_resolution;

    for (const auto& config : configs) {
        Analyzer analyzer(config.filename, config.histname, config.ftype);
        analyzer.setUpperLowerBound(config.chn_lower_bound, config.chn_upper_bound);
        analyzer.setPeakUpperLower(config.peak_lower, config.peak_upper);
        analyzer.setFitParameters(config.p0, config.p1, config.p2, config.p3, config.p4, config.p5, config.p6, config.p7, config.p8, config.p9, config.p10);
        analyzer.setActivity();
        analyzer.setTotalTime();
        analyzer.printActivity();
        analyzer.plot();
        analyzer.pulser(config.pulser_min, config.pulser_max);
        analyzer.setRes();
        analyzer.trapefficiency_redux(config.m);
        analyzer.normefficiency();
        analyzer.ZTestEfficiencies();
        analyzer.printRefResolution();
        yValues_resolution.push_back(analyzer.getRefResolution());
        canvases.push_back(analyzer.getCanvas());
        analyzer.saveResults();
        configCount++;
        processFitParameters(config, analyzer, xValues, errxValues, outfile);
    }

    if (!onlyOneElement) {
        GraphPlotter plotter(yValues, 4);
        plotter.setFitFunction("linear", "[0] + [1] * x", 0, 2300);
        plotter.addData(xValues, errxValues);

        for (int i = 0; i < configCount / analysisPerFile; ++i) {
            plotter.plotAndFit(i);
            plotter.setAndPrintResidues(i);
            plotter.saveResults(i);
            plotter.plotResidues(i);
        }
    }    

    //plotAndFitResolutions(yValues_resolution);

    createSpreadsheet();
    outfile.close();

}

/**
 * @brief A function to get how many years have passed since the source's manufacturing date
 * @param date the source's manufacturing date
 * @return years_passed How many years have passed
 */
double getHowManyYears(const std::string& start_date_str, const std::string& end_date_str) {
    std::tm start_date = {};
    std::istringstream ss_start(start_date_str);
    ss_start >> std::get_time(&start_date, "%d/%m/%Y");

    if (ss_start.fail()) {
        std::cerr << "Failed to parse date!" << std::endl;
        return -1;
    }

    std::tm end_date = {};
    std::istringstream ss_end(end_date_str);
    ss_end >> std::get_time(&end_date, "%d/%m/%Y");

    if (ss_end.fail()) {
        std::cerr << "Failed to parse date!" << std::endl;
        return -1;
    }

    auto start_time = std::chrono::system_clock::from_time_t(std::mktime(&start_date));
    auto end_time = std::chrono::system_clock::from_time_t(std::mktime(&end_date));

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    double years_passed = duration.count() / (365.25*60*60*24);

    std::cout << "Years passed between " << start_date_str << " and " << end_date_str << ": " << years_passed << " years." << std::endl;

    return years_passed;

}

/**
 * @brief Function to get the last number from the histogram directory (used for indexing)
 * @param str The directory (or string)
 * @return the last number in the string
 */
int extractLastNumber(const std::string& str) {
    // Find the last underscore
    size_t pos = str.find_last_of('_');

    // Extract the substring after the last underscore and convert it to an integer
    if (pos != std::string::npos) {
        return std::stoi(str.substr(pos + 1));
    }

    // If no underscore is found or no number is present, return a default value
    return -1;
}

/**
 * @brief Function to get the output file directory
 * @param folder The folder in which the file goes
 * @param filename the file name
 * @param histname the histogram name
 * @return the whole path
 */
std::string getOutputFilePath(std::string& folder, std::string& filename, std::string& histname) {
    size_t lastSlashPos_f = filename.find_last_of("/");
    size_t lastDotPos_f = filename.find_last_of(".");
    std::string outputFileName_f = filename.substr(lastSlashPos_f + 1, lastDotPos_f - lastSlashPos_f - 1);

    size_t lastSlashPos_h = histname.find_last_of("/");
    std::string outputFileName_h = histname.substr(lastSlashPos_h + 1, -1);

    std::string outputFilePath = "../../../" + folder + outputFileName_f + "_" + outputFileName_h;

    return outputFilePath;
}

/**
 * @brief Function to extract the value from the string in .txt files containing results
 * @param line The line to parse
 * @param label The label that should be sought
 * @return The value corresponding to the line
 */
double extractValue(const std::string& line, const std::string& label) {
    size_t pos = line.find(label);
    if (pos != std::string::npos) {
        std::stringstream ss(line.substr(pos + label.size()));
        double value;
        ss >> value;
        return value;
    }
    return 0.0;
}

/**
 * @brief Function to extract the value/uncertainty pair from .txt files
 * @param line The line to parse
 * @param label The label that should be sought
 * @return The value/uncertainty pair
 */
std::pair<double, double> extractValueWithUncertainty(const std::string& line, const std::string& label) {
    size_t pos = line.find(label);
    if (pos != std::string::npos) {
        std::string valueStr = line.substr(pos + label.size());
        std::regex valueRegex(R"(([-+]?[0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?) \+/- ([-+]?[0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?))");
        std::smatch match;
        if (std::regex_search(valueStr, match, valueRegex)) {
            double value = std::stod(match[1].str());
            double uncertainty = std::stod(match[2].str());
            return { value, uncertainty };
        }
    }
    return { 0.0, 0.0 };  // Return default if no match found
}

/**
 * @brief Function to write all values from a single .txt file into the .csv spreadsheet
 * @param inputFile The input .txt file
 * @param csvFile The .csv spreadsheet
 */
void processSpreadsheetFile(const std::string& inputFile, std::ofstream& csvFile) {
    std::ifstream file(inputFile);
    std::string line;

    size_t lastSlash = inputFile.find_last_of("/\\");
    std::string shortenedFileName = inputFile.substr(lastSlash + 1);

    std::pair<double, double> slope, intercept, norm1, mean1, stddev1, norm2, mean2, stddev2, efficiency1, efficiency2, trap_efficiency, res1, res2;
    double pValue = 0;
    double pulserIntegral = 0, totalTime = 0, liveTimePercentage = 0, activity = 0;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Extract values and uncertainties based on labels
            //these will have to be modified when uncertainties are implemented
            if (line.find("Slope") != std::string::npos) slope = extractValueWithUncertainty(line, "Slope : ");
            else if (line.find("Y-intercept") != std::string::npos) intercept = extractValueWithUncertainty(line, "Y-intercept : ");
            else if (line.find("Normalization 1") != std::string::npos) norm1 = extractValueWithUncertainty(line, "Normalization 1 : ");
            else if (line.find("Mean value 1") != std::string::npos) mean1 = extractValueWithUncertainty(line, "Mean value 1 : ");
            else if (line.find("Standard Deviation 1") != std::string::npos) stddev1 = extractValueWithUncertainty(line, "Standard Deviation 1 : ");
            else if (line.find("Normalization 2") != std::string::npos) norm2 = extractValueWithUncertainty(line, "Normalization 2 : ");
            else if (line.find("Mean value 2") != std::string::npos) mean2 = extractValueWithUncertainty(line, "Mean value 2 : ");
            else if (line.find("Standard Deviation 2") != std::string::npos) stddev2 = extractValueWithUncertainty(line, "Standard Deviation 2 : ");
            else if (line.find("Fit p-value") != std::string::npos) pValue = extractValue(line, "Fit p-value: ");  // Single value, no uncertainty
            else if (line.find("Pulser integral") != std::string::npos) pulserIntegral = extractValue(line, "Pulser integral: ");  // Single value
            else if (line.find("Total time for measurements") != std::string::npos) totalTime = extractValue(line, "Total time for measurements: ");
            else if (line.find("Live time percentage") != std::string::npos) liveTimePercentage = extractValue(line, "Live time percentage: ");
            else if (line.find("Activity for the source today") != std::string::npos) activity = extractValue(line, "Activity for the source today: ");
            else if (line.find("Efficiency value for first peak") != std::string::npos) efficiency1 = extractValueWithUncertainty(line, "Efficiency value for first peak: ");
            else if (line.find("Efficiency value for second peak") != std::string::npos) efficiency2 = extractValueWithUncertainty(line, "Efficiency value for second peak: ");
            else if (line.find("Efficiency value with trap method") != std::string::npos) trap_efficiency = extractValueWithUncertainty(line, "Efficiency value with trap method: ");
            else if (line.find("Resolution for first peak") != std::string::npos) res1 = extractValueWithUncertainty(line, "Resolution for first peak: ");
            else if (line.find("Resolution for second peak") != std::string::npos) res2 = extractValueWithUncertainty(line, "Resolution for second peak: ");

        }
        file.close();
        // Write extracted data as a row in the CSV file, including uncertainties in separate columns
        //csvFile << std::scientific << std::setprecision(6);

        csvFile << shortenedFileName << "," << slope.first << "," << slope.second << ","  // Value and uncertainty
            << intercept.first << "," << intercept.second << ","
            << norm1.first << "," << norm1.second << ","
            << mean1.first << "," << mean1.second << ","
            << stddev1.first << "," << stddev1.second << ","
            << norm2.first << "," << norm2.second << ","
            << mean2.first << "," << mean2.second << ","
            << stddev2.first << "," << stddev2.second << ","
            << pValue << ","  // Single value
            << pulserIntegral << "," << totalTime << "," << liveTimePercentage << ","
            << activity << ","
            << efficiency1.first << "," << efficiency1.second << ","
            << efficiency2.first << "," << efficiency2.second << ","
            << trap_efficiency.first << "," << trap_efficiency.second << ","
            << res1.first << "," << res1.second << ","
            << res2.first << "," << res2.second << "\n";
    }
    
}

/**
 * @brief The function to create the whole spreadsheet
 */
void createSpreadsheet() {
    std::string big_results_filename = "../../../out/results_spreadsheet.csv";
    std::ofstream csvFile(big_results_filename);

    csvFile << "File Name,Slope,ErrSlope,Intercept,ErrIntercept,Normalization 1,ErrNormalization 1,Mean 1,ErrMean 1,Standard Deviation 1,ErrStandard Deviation 1,Normalization 2,ErrNormalization 2,"
        << "Mean 2,ErrMean 2,Standard Deviation 2,ErrStandard Deviation 2,Fit p-value,Pulser Integral,Total Time [s],Live Time Percentage,"
        << "Activity [Bq],Efficiency 1,ErrEfficiency 1,Efficiency 2,ErrEfficiency 2,Efficiency Trap,ErrEfficiency Trap,Resolution1,ErrResolution1,Resolution2,ErrResolution2\n";

    std::vector<std::string> inputFiles = {
        //"run1775_coinc_h_EBGO_ADC_1.txt", "run1775_coinc_h_EBGO_ADC_2.txt", "run1775_coinc_h_EBGO_ADC_3.txt", "run1775_coinc_h_EBGO_ADC_4.txt", "run1775_coinc_h_EBGO_ADC_5.txt", "run1775_coinc_h_EBGO_ADC_6.txt",
        "../../../out/fit results/run1776_coinc_h_EBGO_ADC_1.txt", "../../../out/fit results/run1776_coinc_h_EBGO_ADC_2.txt", "../../../out/fit results/run1776_coinc_h_EBGO_ADC_3.txt", "../../../out/fit results/run1776_coinc_h_EBGO_ADC_4.txt", "../../../out/fit results/run1776_coinc_h_EBGO_ADC_5.txt", "../../../out/fit results/run1776_coinc_h_EBGO_ADC_6.txt",
        "../../../out/fit results/run1777_coinc_h_EBGO_ADC_1.txt", "../../../out/fit results/run1777_coinc_h_EBGO_ADC_2.txt", "../../../out/fit results/run1777_coinc_h_EBGO_ADC_3.txt", "../../../out/fit results/run1777_coinc_h_EBGO_ADC_4.txt", "../../../out/fit results/run1777_coinc_h_EBGO_ADC_5.txt", "../../../out/fit results/run1777_coinc_h_EBGO_ADC_6.txt",
        //"run1778_coinc_h_EBGO_ADC_1.txt",
        "../../../out/fit results/run1779_coinc_h_EBGO_ADC_1.txt", "../../../out/fit results/run1779_coinc_h_EBGO_ADC_2.txt", "../../../out/fit results/run1779_coinc_h_EBGO_ADC_3.txt", "../../../out/fit results/run1779_coinc_h_EBGO_ADC_4.txt", "../../../out/fit results/run1779_coinc_h_EBGO_ADC_5.txt", "../../../out/fit results/run1779_coinc_h_EBGO_ADC_6.txt",
        "../../../out/fit results/run1780_coinc_h_EBGO_ADC_1.txt", "../../../out/fit results/run1780_coinc_h_EBGO_ADC_2.txt", "../../../out/fit results/run1780_coinc_h_EBGO_ADC_3.txt", "../../../out/fit results/run1780_coinc_h_EBGO_ADC_4.txt", "../../../out/fit results/run1780_coinc_h_EBGO_ADC_5.txt", "../../../out/fit results/run1780_coinc_h_EBGO_ADC_6.txt",
    };

    for (const auto& file : inputFiles) {
        processSpreadsheetFile(file, csvFile);
    }

    csvFile.close();
    std::cout << "Spreadsheet CSV file generated: " << big_results_filename << std::endl;

}

void runSimulation(const std::string& filename, const std::string& treename, const std::string& branchname) {
    Simulation simulation(filename, treename, branchname);
    simulation.loadTree();
    //simulation.analyzeTree();
    simulation.scanTree();
}

void createEdepHistogram() {
    // Step 1: Open the ROOT file
    TFile* file = TFile::Open("../../../simulations/137-Cs/SimLuna_137Cs.root");
    
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open file!" << std::endl;
        return;
    }

    // Step 2: Access the TTree
    TTree* tree = (TTree*)file->Get("Tree1"); // Replace with your actual tree name
    if (!tree) {
        std::cerr << "Failed to get TTree!" << std::endl;
        return;
    }

    // Step 3: Create a histogram for Edep
    TH1F* histEdep = new TH1F("histEdep", "Histogram of Edep;Edep (units);Entries", 2500, 0, 2500); // Adjust bins and range as needed

    tree->Draw("Edep>>histEdep");

    // Step 5: Draw the histogram
    TCanvas* canvas = new TCanvas("canvas", "Edep Histogram", 800, 600);
    canvas->SetLogy();
    histEdep->Draw();

    // Optionally save the histogram to a file
    canvas->SaveAs("../../../out/EdepHistogram.pdf");
}

void plotAndFitResolutions(std::vector<double>& resolutions) {
    // x-axis values (fixed for all graphs)
    double x_values[8] = { 511, 765, 1384.37, 2375.72, 5180.51, 6171.86, 6791.23, 7556.23 };

    // Open file for output
    std::ofstream outputFile("../../../out/resolution.txt");
    outputFile << "Channel\tSlope \tErrSlope\tIntercept \tErrIntercept \tpValue\n";

    // Check if the resolutions vector has 48 elements
    if (resolutions.size() != 48) {
        std::cerr << "Error: 'resolutions' vector must contain exactly 48 elements." << std::endl;
        return;
    }

    TCanvas* c = new TCanvas("c", "Resolution Graphs", 800, 600);
    c->Divide(3, 2); // Divide canvas into 6 pads

    // Create 6 TGraphs, each with 8 points
    for (int i = 0; i < 6; ++i) {
        c->cd(i + 1); // Switch to the next pad
        TGraph* graph = new TGraph(8, x_values, &resolutions[i * 8]);

        // Set graph title and axis labels
        graph->SetTitle(Form("Channel %d", i + 1));
        graph->GetXaxis()->SetTitle("Energy [keV]");
        graph->GetYaxis()->SetTitle("Resolution");

        // Fit the function y = a + b / sqrt(x)
        TF1* fitFunc = new TF1("fitFunc", "[0] + [1]/sqrt(x) + [2]*x", 500, 8000); // Define fit range
        graph->Fit(fitFunc);

        // Draw graph and fit
        graph->Draw("AP");
        graph->SetMarkerStyle(21);

        // Get fit parameters
        double a = fitFunc->GetParameter(0);
        double a_err = fitFunc->GetParError(0);
        double b = fitFunc->GetParameter(1);
        double b_err = fitFunc->GetParError(1);
        double p = fitFunc->GetProb();

        TLegend* legend = new TLegend(0.6, 0.7, 0.95, 0.95); // Top-right corner
        legend->SetBorderSize(2);  
        //legend->SetFillStyle(0);    // Transparent fill
        legend->SetTextSize(0.03);
        legend->AddEntry((TObject*)0, Form("p0 = %.3f #pm %.3f", a, a_err), ""); // Add p0
        legend->AddEntry((TObject*)0, Form("p1 = %.2f #pm %.2f", b, b_err), ""); // Add p1
        legend->Draw();             // Draw the legend

        // Save results to file
        outputFile << "Graph " << i + 1 << "\t" << a << "\t" << a_err << "\t" << b << "\t\t\t" << b_err << "\t\t\t" << p << "\n";

        // Save each graph as an image
        TString graphFileName = Form("../../../out/resolution graphs/Graph_%d.pdf", i + 1);
        c->Update();
        c->SaveAs(graphFileName); // Save the current canvas/pad
    }

    // Save the entire canvas with all graphs
    c->SaveAs("../../../out/resolution graphs/ResolutionGraphs.pdf"); // Save the canvas with all graphs displayed

    // Close output file
    outputFile.close();

    // Update canvas to display the plots
    c->Update();
}

void analyzeCaesiumSimulations() {

    std::string outputFilePath = "../../../out/Caesium_Simulation_results.txt";
    std::ofstream outputFile(outputFilePath);
    outputFile << "File name\t" << "Efficiency (661 keV)\t" << "Resolution (661 keV)\t" << std::endl;

        TString file_to_load = "../../../macros/SimLuna_137Cs_histo.root";

        TFile* f_in = new TFile(file_to_load);      // open input file

        if (!f_in || f_in->IsZombie())              //check if file exists, if not return error
        {
            std::cout << "Failed to load " << std::endl;
        }

        for (int j = 0; j <= 5; ++j) {
            TH1F* sim_hist = dynamic_cast<TH1F*>(f_in->Get(Form("h_BGO%d_res", j)));
            //sim_hist->GetXaxis()->SetRangeUser(600, 750);


            if (!sim_hist || sim_hist->GetEntries() == 0) {
                std::cerr << "Failed to retrieve histogram!" << std::endl;
                f_in->Close();
                return;
            }

            TCanvas* canv = new TCanvas("cssim", "Caesium simulation", 800, 600);
            canv->SetLogy();

            TF1* peak = new TF1("f1", "[0]*x + [1] + gausn(2)", 640, 690);
            peak->SetParameter(0, -0.01);
            peak->SetParameter(1, 0);
            peak->SetParameter(2, 80000);
            peak->SetParameter(3, 661);
            peak->SetParameter(4, 40);

            sim_hist->Fit("f1");
            
            sim_hist->Draw();
            peak->Draw("SAME");
            canv->Update();
            canv->SaveAs(Form("../../../out/simulation graphs/SimLuna_h_BGO_res%d_Caesium.pdf", j));

            std::cout << sim_hist->GetEntries() << std::endl;

            std::cout << "Efficiency for this simulated detector: " << peak->GetParameter(2) / (0.85* sim_hist->GetEntries()) << std::endl;
            outputFile << file_to_load << "\t" << peak->GetParameter(2) / (0.85 * sim_hist->GetEntries()) << "+/-" << peak->GetParError(2) / (0.85 * sim_hist->GetEntries()) << "\t"
               << peak->GetParameter(4) / peak->GetParameter(3) << std::endl;

            //delete canv;
        

    }
    outputFile.close();
}

void analyzeCobaltSimulations() {

    std::string outputFilePath = "../../../out/Cobalt_Simulation_results.txt";
    std::ofstream outputFile(outputFilePath);
    outputFile << "File name\t" << "Efficiency (1173 keV)\t" << "Efficiency (1332 keV)\t" << "Resolution (1173 keV)\t" << "Resolution (1332 keV)\t" << std::endl;

    TString file_to_load = "../../../macros/SimLuna_60Co_histo.root";

    TFile* f_in = new TFile(file_to_load);      // open input file

    if (!f_in || f_in->IsZombie())              //check if file exists, if not return error
    {
        std::cout << "Failed to load " << std::endl;
    }

    for (int j = 0; j <= 5; ++j) {
        TH1F* sim_hist = dynamic_cast<TH1F*>(f_in->Get(Form("h_BGO%d_res", j)));
        sim_hist->GetXaxis()->SetRangeUser(1000, 1450);


        if (!sim_hist || sim_hist->GetEntries() == 0) {
            std::cerr << "Failed to retrieve histogram!" << std::endl;
            f_in->Close();
            return;
        }

        TCanvas* canv = new TCanvas("cosim", "Cobalt simulation", 800, 600);
        canv->SetLogy();

        TF1* peak1 = new TF1("f1", "[0]*x + [1] + gausn(2) + gausn(5)", 1150, 1350);
        peak1->SetParameter(0, 0);
        peak1->SetParameter(1, 0);
        peak1->SetParameter(2, 40000);
        peak1->SetParameter(3, 1173);
        peak1->SetParameter(4, 40);
        peak1->SetParameter(5, 40000);
        peak1->SetParameter(6, 1332);
        peak1->SetParameter(7, 40);

        sim_hist->Fit("f1");
        sim_hist->Draw();
        peak1->Draw("SAME");
        canv->Update();
        canv->SaveAs(Form("../../../out/simulation graphs/SimLuna_h_BGO_res%d_Cobalt.pdf", j));

        std::cout << "Efficiencies for this simulated detector: " << peak1->GetParameter(2) / 1e7 << ", " << peak1->GetParameter(5) / 1e7 << std::endl;
        outputFile << file_to_load << "\t" << peak1->GetParameter(2) / 1e7 << "+/-" << peak1->GetParError(2) / 1e7
            << "\t" << peak1->GetParameter(5) / 1e7 << "+/-" << peak1->GetParError(5) / 1e7 << "\t" << "\t" <<
            peak1->GetParameter(4) / peak1->GetParameter(3) << "\t" << peak1->GetParameter(7) / peak1->GetParameter(6) << "\t" << std::endl;

        delete canv;
    }
    outputFile.close();
}

void compareSimExpHistograms() {
    TFile* file_exp_cs = new TFile("../../../root files/run1776_coinc.root");
    TFile* file_sim_cs = new TFile("../../../macros/SimLuna_137Cs_histo.root");

    if (!file_exp_cs || file_exp_cs->IsZombie() || !file_sim_cs || file_sim_cs->IsZombie())              //check if file exists, if not return error
    {
        std::cout << "Failed to load " << std::endl;
    }

    TCanvas* canvas_cs = new TCanvas("cs comp", "Caesium experimental-simulated comparison", 800, 600);
    canvas_cs->SetLogy();
    TLegend* legend_cs = new TLegend(0.7, 0.7, 0.9, 0.9);
    TH1F* exp_hist_cs = dynamic_cast<TH1F*>(file_exp_cs->Get("Energy/h_EBGO_4"));
    TH1F* sim_hist_cs = dynamic_cast<TH1F*>(file_sim_cs->Get("h_BGO4_res"));
    exp_hist_cs->GetXaxis()->SetRangeUser(0, 1000);
    sim_hist_cs->GetXaxis()->SetRangeUser(0, 1000);
    exp_hist_cs->SetLineColor(kGreen);
    sim_hist_cs->SetLineColor(kRed);
    legend_cs->AddEntry(exp_hist_cs, "Experimental", "f");
    legend_cs->AddEntry(sim_hist_cs, "Simulated", "f");
    
    sim_hist_cs->Draw();
    exp_hist_cs->Draw("SAME");
    
    legend_cs->Draw();
    canvas_cs->Update();
    canvas_cs->SaveAs("../../../out/comparison graphs/Caesium.pdf");

    file_exp_cs->Close();
    file_sim_cs->Close();

    TFile* file_exp_co = new TFile("../../../root files/run1777_coinc.root");
    TFile* file_sim_co = new TFile("../../../macros/SimLuna_60Co_histo.root");

    if (!file_exp_co || file_exp_co->IsZombie() || !file_sim_co || file_sim_co->IsZombie())              //check if file exists, if not return error
    {
        std::cout << "Failed to load " << std::endl;
    }

    TCanvas* canvas_co = new TCanvas("co comp", "Cobalt experimental-simulated comparison", 800, 600);
    canvas_co->SetLogy();
    TLegend* legend_co = new TLegend(0.7, 0.7, 0.9, 0.9);

    TH1F* exp_hist_co = dynamic_cast<TH1F*>(file_exp_co->Get("Energy/h_EBGO_4"));
    TH1F* sim_hist_co = dynamic_cast<TH1F*>(file_sim_co->Get("h_BGO4_res"));
    exp_hist_co->GetXaxis()->SetRangeUser(0, 2800);
    sim_hist_co->GetXaxis()->SetRangeUser(0, 2800);
    exp_hist_co->SetLineColor(kGreen);
    sim_hist_co->SetLineColor(kRed);
    legend_co->AddEntry(exp_hist_co, "Experimental", "f");
    legend_co->AddEntry(sim_hist_co, "Simulated", "f");

    sim_hist_co->Draw();

    exp_hist_co->Draw("SAME");
    legend_co->Draw();

    canvas_co->Update();
    canvas_co->SaveAs("../../../out/comparison graphs/Cobalt.pdf");

    file_exp_co->Close();
    file_sim_co->Close();
}