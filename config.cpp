#include "config.h"
#include "commonDLL.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/foreach.hpp>
#include <filesystem>

void sConfig::load(const std::string &filename)
{
	boost::property_tree::ptree pt;

	std::tr2::sys::path myfile(filename);
	if (std::tr2::sys::exists(myfile))
	{
		read_xml(filename, pt);

		configVersion = pt.get<uint32_t>("config.version");

		//BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("config.modules"))
			//m_modules.insert(v.second.data());
	}
	else
		configVersion = COMMONDLL_API_VER;
}

void sConfig::save(const std::string &filename)
{
	boost::property_tree::ptree pt;

	pt.put("config.version", configVersion);

	//BOOST_FOREACH(const std::string &name, m_modules)
		//pt.put("config.modules.module", name, true);

	write_xml(filename, pt);
}