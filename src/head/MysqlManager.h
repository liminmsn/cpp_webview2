#pragma once
#include "IDataSource.h"
#include "string"

class MysqlManager : public IDataSource
{
public:
    MysqlManager();
    ~MysqlManager() override;
    bool Start() override;
    bool Stop() override;
    bool Init() override;
private:
    std::string zipPath;
    std::string outDir;
};