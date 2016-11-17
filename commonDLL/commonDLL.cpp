#include "commonDLL.h"

namespace commondll {

COMMONDLL_API CommonDLL& CommonDLL::instance()
{
	static CommonDLL instance;
	return instance;
}

COMMONDLL_API void CommonDLL::addName(const std::string& name)
{
	m_names.push_back(name);
}

COMMONDLL_API const std::vector<std::string>& CommonDLL::getNames() const
{
	return m_names;
}

CommonDLL::~CommonDLL()
{
	m_names.clear();
}

} // namespace commondll