#ifndef CONFIG_H
#define CONFIG_H

#include <analyzer.h>

class Config {
public:
	
	std::string filename;
	std::string histname;
	Analyzer::FuncType ftype;
	double p0, p1, p2, p3, p4, p5, p6, p7;
	int chn_lower_bound, chn_upper_bound, m;
	
};

#endif // CONFIG_H