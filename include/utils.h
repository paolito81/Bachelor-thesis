#pragma once
#include <TFile.h>
#include <TH1F.h>

void display(std::string filename);
void isTFileOpen(TFile* inFile);
static double trap_area(TH1F* histogram, int chn_1, int chn_2);
static double var_peak(double area, double trap, int n, int m);