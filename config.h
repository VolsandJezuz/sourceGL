#ifndef CONFIG_H
#define CONFIG_H

#include "c99defs.h"
#include <string>
#include <vector>

#ifdef SOURCEGAMELOUNGE_EXPORTS
#define SOURCEGAMELOUNGE_API __declspec(dllexport)
#else
#define SOURCEGAMELOUNGE_API __declspec(dllimport)
#endif // SOURCEGAMELOUNGE_EXPORTS

namespace sGL {

class SOURCEGAMELOUNGE_API config
{
public:
	static config& instance();
	void load(const std::string &filename);
	void save(const std::string &filename);

	uint32_t configVersion;
	uint32_t steam32ID;
	std::vector<std::string>* games;
	bool optOutSteamID;
	bool runAtWindowsStartup;
	bool minToTrayStartup;
	bool autoExit;

private:
	config();
	~config();
};

} // namesapce sGL

#endif // CONFIG_H