#pragma once
#include <string>

class IDataSource
{
public:
	virtual ~IDataSource() = default;
	virtual void Init() = 0;
public:
	std::string zipPath;
	std::string outDir;
	std::string GetOutDir() const{
		return outDir;
	}
};