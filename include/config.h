#ifndef CONFIG_H
#define CONFIG_H

#include <analyzer.h>

struct Config {
	
	std::string filename;
	std::string histname;
	Analyzer::FuncType ftype;
	int chn_lower_bound, chn_upper_bound, m;
	double p0, p1, p2, p3, p4, p5, p6, p7;
	
	void getEnergies();
};

#endif // CONFIG_H