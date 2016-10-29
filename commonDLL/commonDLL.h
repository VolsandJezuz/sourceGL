#ifndef _COMMONDLL_H_
#define _COMMONDLL_H_

#define STRICT
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_WINDOWS 0x0501
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <vector>

#ifdef COMMONDLL_EXPORTS
#define COMMONDLL_API __declspec(dllexport)
#else
#define COMMONDLL_API __declspec(dllimport)
#endif

#define COMMONDLL_API_MAJOR_VER 0
#define COMMONDLL_API_MINOR_VER 2
#define COMMONDLL_API_PATCH_VER 0
#define MAKE_SEMANTIC_VERSION(major, minor, patch) \
                             ((major << 24) | \
                              (minor << 16) | \
                               patch        )
#define COMMONDLL_API_VER \
	MAKE_SEMANTIC_VERSION(COMMONDLL_API_MAJOR_VER, \
                              COMMONDLL_API_MINOR_VER, \
                              COMMONDLL_API_PATCH_VER)

typedef std::vector<std::string> nameList;

class commonDLLImpl;

class COMMONDLL_API commonDLL {
public:
	static commonDLL& instance();

	void addName(const std::string& name);
	const nameList& getNames() const;

private:
	commonDLL(void);
	virtual ~commonDLL(void);

	commonDLLImpl* m_pImpl;
};

extern "C" COMMONDLL_API void displayError(LPCWSTR pwszError);

#endif