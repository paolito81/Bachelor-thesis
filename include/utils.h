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