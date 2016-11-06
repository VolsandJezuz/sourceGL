#ifndef PLUGINS_NVIDIA_NVIDIACONFIG_H
#define PLUGINS_NVIDIA_NVIDIACONFIG_H

#include "c99defs.h"
#include "boost/variant.hpp"
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace nvidia {

class NVIDIAconfig
{
public:
	typedef std::map<std::string, std::vector<std::pair<std::string, boost::variant<std::string, int, bool>>>> gameSettings_t;

	static NVIDIAconfig& instance();
	void load(const std::string &filename);
	void save(const std::string &filename);

	uint32_t NVIDIAconfigVersion;
	gameSettings_t gameSettings;

private:
	NVIDIAconfig();
	~NVIDIAconfig() = default;
};

} // namespace nvidia

#endif // PLUGINS_NVIDIA_NVIDIACONFIG_H