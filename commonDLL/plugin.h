#ifndef COMMONDLL_PLUGIN_H_
#define COMMONDLL_PLUGIN_H_

#include "commonDLL.h"
#include <string>

namespace commondll {

class plugin
{
	friend class pluginManager;

public:
	COMMONDLL_API std::wstring getName() const;

protected:
	COMMONDLL_API plugin();
	COMMONDLL_API virtual ~plugin() = default;
	COMMONDLL_API void setName(const std::wstring& name);

private:
	std::wstring m_name;
};

} // namespace commondll

#endif // COMMONDLL_PLUGIN_H_