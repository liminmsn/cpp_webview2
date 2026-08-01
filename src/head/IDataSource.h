#pragma once
#include <string>
#include "../utils/JsonUtil.h"

class IDataSource
{
public:
	virtual ~IDataSource() = default;
	virtual void Init() = 0;
	virtual void OnMessage(json& data) = 0;
public:
	std::string zipPath;
	std::string outDir;
	std::string GetOutDir() const {
		return outDir;
	}
};