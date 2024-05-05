#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include "run1776.h"

void run1776 ()
{
	TCanvas* c1 = new TCanvas();

	TFile* inFile = new TFile("../../../root files/run1776_coinc.root", "read");

	if (!inFile || inFile->IsZombie())
	{
		std::cerr << "Failed to open file: " << "run1776_coinc.root" << std::endl;
		exit(1);
	}

	inFile->Map("forComp");

	//TTree* tree = (TTree*)inFile->Get();

	inFile->Close();

}