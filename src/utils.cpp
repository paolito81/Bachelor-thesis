#include <TFile.h>
#include <TH1F.h>
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
        //analyzer.trapefficiency(config.m);
        analyzer.trapefficiency_redux(config.m);
        analyzer.normefficiency();
        analyzer.ZTestEfficiencies();
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

    std::pair<double, double> slope, intercept, norm1, mean1, stddev1, norm2, mean2, stddev2, efficiency1, efficiency2, trap_efficiency;
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
            << trap_efficiency.first << "," << trap_efficiency.second << "\n";
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
        << "Activity [Bq],Efficiency 1,ErrEfficiency 1,Efficiency 2,ErrEfficiency 2,Efficiency Trap,ErrEfficiency Trap\n";

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