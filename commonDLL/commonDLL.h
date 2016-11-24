#ifndef COMMONDLL_COMMONDLL_H_
#define COMMONDLL_COMMONDLL_H_

#include <string>
#include <vector>

#ifdef COMMONDLL_EXPORTS
#define COMMONDLL_API __declspec(dllexport)
#else
#define COMMONDLL_API __declspec(dllimport)
#endif // COMMONDLL_EXPORTS

namespace commondll {

class CommonDLL
{
public:
	static COMMONDLL_API CommonDLL& instance();
	COMMONDLL_API void addName(const std::string& name);
	COMMONDLL_API const std::vector<std::string>& getNames() const;

private:
	CommonDLL() = default;
	virtual ~CommonDLL();

	std::vector<std::string> m_names;
};

} // namespace commondll

#endif // COMMONDLL_COMMONDLL_H_