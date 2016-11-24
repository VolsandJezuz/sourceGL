#ifndef PLUGINS_NVIDIA_NVIDIACONFIG_H
#define PLUGINS_NVIDIA_NVIDIACONFIG_H

#include "c99defs.h"
#include <boost/variant.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

#ifdef PLUGIN_NVIDIA_EXPORTS
#define PLUGIN_NVIDIA_API __declspec(dllexport)
#else
#define PLUGIN_NVIDIA_API __declspec(dllimport)
#endif // PLUGIN_NVIDIA_EXPORTS

#define PLUGIN_NVIDIA_MAJOR_VER 0
#define PLUGIN_NVIDIA_MINOR_VER 1
#define PLUGIN_NVIDIA_PATCH_VER 3
#define MAKE_SEMANTIC_VERSION(major, minor, patch) ((major << 24) | (minor << 16) | patch)
#define PLUGIN_NVIDIA_VER MAKE_SEMANTIC_VERSION(PLUGIN_NVIDIA_MAJOR_VER, PLUGIN_NVIDIA_MINOR_VER, PLUGIN_NVIDIA_PATCH_VER)

namespace nvidia {

class NVIDIAconfig
{
public:
	typedef std::map<std::string, std::vector<std::pair<std::string, boost::variant<std::string, int, bool>>>> gameSettings_t;

	static PLUGIN_NVIDIA_API NVIDIAconfig& instance();
	void load(const std::string &filename);
	PLUGIN_NVIDIA_API void save(const std::string &filename);

	uint32_t nvidiaConfigVersion;
	gameSettings_t gameSettings;

private:
	NVIDIAconfig();
	~NVIDIAconfig() = default;
};

} // namespace nvidia

#endif // PLUGINS_NVIDIA_NVIDIACONFIG_H