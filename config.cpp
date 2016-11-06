#include "config.h"
#include "commonDLL.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <filesystem>

namespace sGL {

SOURCEGAMELOUNGE_API config& config::instance()
{
	static config config;
	return config;
}

void config::load(const std::string &filename)
{
	boost::property_tree::ptree pt;

	std::tr2::sys::path myfile(filename);
	if (std::tr2::sys::exists(myfile))
	{
		read_xml(filename, pt);

		uint32_t currentConfigVersion = pt.get<uint32_t>("sGL.version");
		if (currentConfigVersion != configVersion)
		{
			// make changes for old COMMONDLL_API_VER
			configVersion = currentConfigVersion;
		}

		steam32ID = pt.get<uint32_t>("sGL.Steam32ID");

		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("sGL.Games"))
			(*games).push_back(v.second.data());

		runAtWindowsStartup = pt.get<bool>("sGL.ProgramSettings.RunAtWindowsStartup", false);
		minToTrayStartup = pt.get<bool>("sGL.ProgramSettings.MinimizeToTrayOnStartup", false);
		autoExit = pt.get<bool>("sGL.ProgramSettings.AutoExitAfterRunningGame", false);
	}
}

void config::save(const std::string &filename)
{
	boost::property_tree::ptree pt;

	pt.put("sGL.version", configVersion);
	pt.put("sGL.Steam32ID", steam32ID);

	BOOST_FOREACH(const std::string &game, *games)
		pt.add("sGL.Games.Game", game);

	pt.put("sGL.ProgramSettings.RunAtWindowsStartup", runAtWindowsStartup);
	pt.put("sGL.ProgramSettings.MinimizeToTrayOnStartup", minToTrayStartup);
	pt.put("sGL.ProgramSettings.AutoExitAfterRunningGame", autoExit);

	write_xml(filename, pt);
}

config::config()
{
	games = new std::vector<std::string>();

	configVersion = COMMONDLL_API_VER;
	// functions to get steam32ID and other default values
}

config::~config()
{
	delete games;
	games = NULL;
}

} // namespace sGL