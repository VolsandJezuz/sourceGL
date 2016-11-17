#include "plugin.h"

namespace commondll {

COMMONDLL_API std::wstring Plugin::getName() const
{
	return m_name;
}

COMMONDLL_API Plugin::Plugin()
{
	CommonDLL::instance().addName("Plugin");
}

COMMONDLL_API void Plugin::setName(const std::wstring& name)
{
	m_name = name;
}

} // namespace commondll