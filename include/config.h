#ifndef CONFIG_H
#define CONFIG_H

#include <analyzer.h>
#include <iostream>
#include <fstream>
#include <filesystem>

struct Config {
	
	std::string filename;
	std::string histname;
	Analyzer::FuncType ftype;
	int chn_lower_bound, chn_upper_bound, m;
	double p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
	

	void printMeanValues(std::ofstream outfile, Analyzer analyzer) const;
};

#endif // CONFIG_H