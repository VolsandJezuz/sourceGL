#include "NVIDIAconfig.h"
#include "config.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <filesystem>

namespace nvidia {

PLUGIN_NVIDIA_API NVIDIAconfig& NVIDIAconfig::instance()
{
	static NVIDIAconfig nvidiaConfig;
	return nvidiaConfig;
}

void NVIDIAconfig::load(const std::string &filename)
{
	boost::property_tree::ptree pt;

	std::tr2::sys::path myfile(filename);
	if (std::tr2::sys::exists(myfile))
	{
		read_xml(filename, pt);

		uint32_t currentNVIDIAconfigVersion = pt.get<uint32_t>("NVIDIA.version");
		if (currentNVIDIAconfigVersion != nvidiaConfigVersion)
		{
			// make changes for old PLUGIN_NVIDIA_VER
			nvidiaConfigVersion = currentNVIDIAconfigVersion;
		}
	}
	// else
		// possibly more function to get default values

	BOOST_FOREACH(const std::string &game, *sgl::Config::instance().getGames())
	{
		std::vector<std::pair<std::string, boost::variant<std::string, int, bool>>> settings =
		{
			{"setting1", pt.get<bool>(("NVIDIA." + game + ".setting1").c_str(), false)},
			{"setting2", pt.get<int>(("NVIDIA." + game + ".setting2").c_str(), 0)},
			{"setting3", pt.get<std::string>(("NVIDIA." + game + ".setting3").c_str(), "none")}
		};

		gameSettings.insert({game, settings});
	}
}

PLUGIN_NVIDIA_API void NVIDIAconfig::save(const std::string &filename)
{
	boost::property_tree::ptree pt;

	pt.put("NVIDIA.version", nvidiaConfigVersion);

	BOOST_FOREACH(gameSettings_t::value_type &game, gameSettings)
	{
		for (std::pair<std::string, boost::variant<std::string, int, bool>> &setting : game.second)
			pt.add(("NVIDIA." + game.first + "." + setting.first).c_str(), setting.second);
	}

	write_xml(filename, pt);
}

NVIDIAconfig::NVIDIAconfig()
{
	nvidiaConfigVersion = PLUGIN_NVIDIA_VER;
	// functions to get default values
}

} // namespace nvidia