#ifndef PULSER.H
#define PULSER.H

#include <iostream>
#include <string>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>

/**
 * @brief Used for pulser analysis for livetime
 */
class Pulser {
public:

private:
    std::string filename; /**< String containing the directory to the .root file */
    std::string histname; /**< String containing the directory to the histogram file */
    TFile* inFile; /**< Pointer to the ROOT file */
    TH1F* histogram; /**< Pointer to the histogram */
};

#endif // !PULSER.H
