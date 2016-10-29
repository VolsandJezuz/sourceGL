#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "commonDLL.h"

class pluginPimpl;

class COMMONDLL_API plugin
{
public:
	plugin(void);
	virtual ~plugin(void);

	std::wstring getName() const;

protected:
	friend class pluginManager;
	void setName(const std::wstring& name);

private:
	pluginPimpl* m_implementation;
};

#endif