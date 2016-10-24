#ifndef CONFIG_H
#define CONFIG_H

#include <libsgl/c99defs.h>
//#include <set>
#include <string>

struct sConfig
{
	uint32_t configVersion;
	//std::set<std::string> m_modules;
	void load(const std::string &filename);
	void save(const std::string &filename);
};

#endif