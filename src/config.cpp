#include <analyzer.h>
#include <config.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include "run1776.h"
#include "utils.h"
#include <fstream>

void Config::getEnergies() {
	if (ftype == Analyzer::F1) {
		return p3;
	}
	else { return p3, p6; }
}