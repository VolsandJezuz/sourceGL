#include "commonDLL.h"

class commonDLLImpl
{
public:
	nameList m_names;
};

commonDLL::commonDLL()
{
	m_pImpl = new commonDLLImpl();
	return;
}

commonDLL::~commonDLL()
{
	m_pImpl->m_names.clear();
	delete m_pImpl;
}

commonDLL& commonDLL::instance()
{
	static commonDLL instance;
	return instance;
}

void commonDLL::addName(const std::string& name)
{
	m_pImpl->m_names.push_back(name);
}

const nameList& commonDLL::getNames() const
{
	return m_pImpl->m_names;
}