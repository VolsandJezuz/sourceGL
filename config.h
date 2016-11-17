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

namespace sgl {

class Config
{
public:
	static SOURCEGAMELOUNGE_API Config& instance();
	SOURCEGAMELOUNGE_API std::vector<std::string>* getGames() const;
	void load(const std::string &filename);
	void save(const std::string &filename);

	uint32_t configVersion;
	uint32_t steam32ID;
	bool optOutSteamID;
	bool runAtWindowsStartup;
	bool minToTrayStartup;
	bool autoExit;

private:
	Config();
	~Config();

	std::vector<std::string>* pGames;
};

} // namesapce sgl

#endif // CONFIG_H