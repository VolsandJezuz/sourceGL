#include "plugin.h"

class pluginPimpl
{
public:
	std::wstring m_name;
};

plugin::plugin(void)
{
	commonDLL::instance().addName("Plugin");

	m_implementation = new pluginPimpl();
}

plugin::~plugin(void)
{
	delete m_implementation;
}

std::wstring plugin::getName() const
{
	return m_implementation->m_name;
}

void plugin::setName(const std::wstring& name)
{
	m_implementation->m_name = name;
}