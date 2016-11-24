#include "config.h"
#include "setup.h"
#include "commonDLL.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <filesystem>

namespace sgl {

SOURCEGAMELOUNGE_API Config& Config::instance()
{
	static Config config;
	return config;
}

SOURCEGAMELOUNGE_API std::vector<std::string>* Config::getGames() const
{
	return pGames;
}

void Config::load(const std::string &filename)
{
	boost::property_tree::ptree pt;

	std::tr2::sys::path myFile(filename);
	if (std::tr2::sys::exists(myFile))
	{
		read_xml(filename, pt);

		uint32_t currentConfigVersion = pt.get<uint32_t>("sGL.version");
		if (currentConfigVersion != configVersion)
		{
			// make changes for old SOURCEGAMELOUNGE_API_VER
			configVersion = currentConfigVersion;
		}

		steam32ID = pt.get<uint32_t>("sGL.Steam32ID");

		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("sGL.Games"))
			pGames->push_back(v.second.data());

		BOOST_FOREACH(const std::string &game, *pGames)
		{
			gameAppIDs.push_back(pt.get<int>(("sGL." + game + ".AppID").c_str()));
			gameFileLocations.push_back(pt.get<std::string>(("sGL." + game + ".FileLocation").c_str()));
		}

		optOutSteamID =			pt.get<bool>("sGL.ProgramSettings.OptOutSteamID");
		runAtWindowsStartup =	pt.get<bool>("sGL.ProgramSettings.RunAtWindowsStartup");
		minToTrayStartup =		pt.get<bool>("sGL.ProgramSettings.MinimizeToTrayOnStartup");
		autoExit =				pt.get<bool>("sGL.ProgramSettings.AutoExitAfterRunningGame");
	}
	else
	{
		pGames->push_back("Hotkey-on-demand");
		gameAppIDs.push_back(0);
		gameFileLocations.push_back("na");
	}
}

void Config::save(const std::string &filename)
{
	boost::property_tree::ptree pt;

	pt.put("sGL.version",									configVersion);
	pt.put("sGL.Steam32ID",									steam32ID);

	int i = 0;
	BOOST_FOREACH(const std::string &game, *pGames)
	{
		pt.add("sGL.Games.Game",							game);
		pt.put(("sGL." + game + ".AppID").c_str(),			gameAppIDs[i]);
		pt.put(("sGL." + game + ".FileLocation").c_str(),	gameFileLocations[i]);
		++i;
	}

	pt.put("sGL.ProgramSettings.OptOutSteamID",				optOutSteamID);
	pt.put("sGL.ProgramSettings.RunAtWindowsStartup",		runAtWindowsStartup);
	pt.put("sGL.ProgramSettings.MinimizeToTrayOnStartup",	minToTrayStartup);
	pt.put("sGL.ProgramSettings.AutoExitAfterRunningGame",	autoExit);

	write_xml(filename, pt);
}

Config::Config()
{
	configVersion = SOURCEGAMELOUNGE_API_VER;

	pGames = new std::vector<std::string>();

	optOutSteamID =			false;
	runAtWindowsStartup =	false;
	minToTrayStartup =		false;
	autoExit =				false;
	// functions to get steam32ID and other default values
}

Config::~Config()
{
	delete pGames;
	pGames = NULL;
}

} // namespace sgl