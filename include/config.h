#ifndef CONFIG_H
#define CONFIG_H

class Config {
public:
	std::string filename;
	std::string histname;
	double p0, p1, p2, p3, p4;
	int chn_lower_bound, chn_upper_bound, m;
};

#endif // CONFIG_H