#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "NVIDIAconfig.h"
#include "NVIDIA.h"
#include "config.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <filesystem>
#include <Windows.h>

namespace nvidia {

NVIDIAconfig& NVIDIAconfig::instance()
{
	static NVIDIAconfig NVIDIAconfig;
	return NVIDIAconfig;
}

void NVIDIAconfig::load(const std::string &filename)
{
	boost::property_tree::ptree pt;

	std::tr2::sys::path myfile(filename);
	if (std::tr2::sys::exists(myfile))
	{
		read_xml(filename, pt);

		uint32_t currentNVIDIAconfigVersion = pt.get<uint32_t>("NVIDIA.version");
		if (currentNVIDIAconfigVersion != NVIDIAconfigVersion)
		{
			// make changes for old PLUGIN_NVIDIA_VER
			NVIDIAconfigVersion = currentNVIDIAconfigVersion;
		}

		BOOST_FOREACH(const std::string &game, *sGL::config::instance().games)
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
}

void NVIDIAconfig::save(const std::string &filename)
{
	boost::property_tree::ptree pt;

	pt.put("NVIDIA.version", NVIDIAconfigVersion);

	BOOST_FOREACH(gameSettings_t::value_type &game, gameSettings)
	{
		for (std::pair<std::string, boost::variant<std::string, int, bool>> &setting : game.second)
			pt.add(("NVIDIA." + game.first + "." + setting.first).c_str(), setting.second);
	}

	write_xml(filename, pt);
}

NVIDIAconfig::NVIDIAconfig()
{
	NVIDIAconfigVersion = PLUGIN_NVIDIA_VER;
	// functions to get default values
}

} // namespace nvidia