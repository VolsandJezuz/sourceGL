#ifndef SETUP_H
#define SETUP_H

#include <string>
#include <vector>

namespace sgl {

class Setup
{
public:
	static Setup& instance();
	void initialSetup();

	std::wstring steamDir;

private:
	Setup() = default;
	~Setup() = default;
	bool findSteamDir();
	std::vector<std::wstring> enumSteamIDs();
};

} // namesapce sgl

#endif // SETUP_H