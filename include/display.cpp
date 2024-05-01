#include <iostream> 
#include <iomanip>
#include <fstream>
#include <string>
#include <filesystem>

const std::filesystem::path welcomeFilePath{ "../welcome.txt" };


void display(std::string filename) {
    std::string displaystr;
    std::ifstream inFile;

    inFile.open(welcomeFilePath);
    if (!inFile.is_open()) {
        std::cout << "Unable to open file";
        //throw std::runtime_error("Error opening welcome.txt");
        exit(1);
    }

    while (getline(inFile,displaystr)) {
        std::cout << displaystr << std::endl;
    }

    inFile.close();
}