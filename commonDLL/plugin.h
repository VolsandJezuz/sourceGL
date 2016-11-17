#ifndef COMMONDLL_PLUGIN_H_
#define COMMONDLL_PLUGIN_H_

#include "commonDLL.h"
#include <string>

namespace commondll {

class Plugin
{
	friend class PluginManager;

public:
	COMMONDLL_API std::wstring getName() const;

protected:
	COMMONDLL_API Plugin();
	COMMONDLL_API virtual ~Plugin() = default;
	COMMONDLL_API void setName(const std::wstring& name);

private:
	std::wstring m_name;
};

} // namespace commondll

#endif // COMMONDLL_PLUGIN_H_