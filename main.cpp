#include <iostream>
#include <display.cpp>
#include <string>
#include <TF1.h>
#include <TH1.h>

int main (){
    display("welcome");

    TF1* gauss_background = new TF1("gauss background", "[0]*x + [1]", 0, 20);
    TH1* histogram = new TH1("histo", "histogram", 10, 0, 100);

    return 0;
}