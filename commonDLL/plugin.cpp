#include "plugin.h"

plugin::plugin(void)
{
	commonDLL::instance().addName("Plugin");
}

plugin::~plugin(void)
{
}

std::wstring plugin::getName() const
{
	return m_name;
}

void plugin::setName(const std::wstring& name)
{
	m_name = name;
}