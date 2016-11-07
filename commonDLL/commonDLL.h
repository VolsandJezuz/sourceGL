#ifndef COMMONDLL_COMMONDLL_H_
#define COMMONDLL_COMMONDLL_H_

#include <string>
#include <vector>

#ifdef COMMONDLL_EXPORTS
#define COMMONDLL_API __declspec(dllexport)
#else
#define COMMONDLL_API __declspec(dllimport)
#endif // COMMONDLL_EXPORTS

#define COMMONDLL_API_MAJOR_VER 0
#define COMMONDLL_API_MINOR_VER 2
#define COMMONDLL_API_PATCH_VER 2
#define MAKE_SEMANTIC_VERSION(major, minor, patch) ((major << 24) | (minor << 16) | patch)
#define COMMONDLL_API_VER MAKE_SEMANTIC_VERSION(COMMONDLL_API_MAJOR_VER, COMMONDLL_API_MINOR_VER, COMMONDLL_API_PATCH_VER)

namespace commondll {

class commonDLL
{
public:
	static COMMONDLL_API commonDLL& instance();
	COMMONDLL_API void addName(const std::string& name);
	COMMONDLL_API const std::vector<std::string>& getNames() const;

private:
	std::vector<std::string> m_names;

	commonDLL() = default;
	virtual ~commonDLL();
};

} // namespace commondll

#endif // COMMONDLL_COMMONDLL_H_