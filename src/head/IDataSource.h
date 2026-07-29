#pragma once
#include <string>

class IDataSource
{
public:
	virtual ~IDataSource() = default;
	virtual bool Start() = 0;
	virtual bool Stop() = 0;
	virtual bool Init() = 0;
};