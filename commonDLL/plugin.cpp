#include "plugin.h"

namespace commondll {

COMMONDLL_API plugin::plugin()
{
	commonDLL::instance().addName("Plugin");
}

COMMONDLL_API std::wstring plugin::getName() const
{
	return m_name;
}

COMMONDLL_API void plugin::setName(const std::wstring& name)
{
	m_name = name;
}

} // namespace commondll