#include "commonDLL.h"

namespace commondll {

commonDLL::~commonDLL()
{
	m_names.clear();
}

COMMONDLL_API commonDLL& commonDLL::instance()
{
	static commonDLL instance;
	return instance;
}

COMMONDLL_API void commonDLL::addName(const std::string& name)
{
	m_names.push_back(name);
}

COMMONDLL_API const std::vector<std::string>& commonDLL::getNames() const
{
	return m_names;
}

} // namespace commondll