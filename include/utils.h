#pragma once
#include <TFile.h>
#include <TH1F.h>
#include <vector>
#include <analyzer.h>
#include <config.h>

void display(std::string filename);
void isTFileOpen(TFile* inFile);
double trap_area(TH1F* histogram, int chn_1, int chn_2, int m);
double var_peak(double area, double trap, int n, int m);

void runAnalysis(const std::vector<Config>& configs, bool onlyOneElement);
double getHowManyYears(const std::string& date);
int extractLastNumber(const std::string& str);
std::string getOutputFilePath(std::string& folder, std::string& filename, std::string& histname);
std::pair<double, double> extractValueWithUncertainty(const std::string& line, const std::string& label);
double extractValue(const std::string& line, const std::string& label);
void processSpreadsheetFile(const std::string& inputFile, std::ofstream& csvFile);
void createSpreadsheet();