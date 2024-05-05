#include <iostream>
#include <utils.h>
#include <string>
#include <TF1.h>
#include <TH1F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <run1776.h>
#include <TApplication.h>

int main (int argc, char* argv[]) {
    TApplication app("app", &argc, argv);

    display("welcome");

    run1776();

    app.Run();
    return 0;
}