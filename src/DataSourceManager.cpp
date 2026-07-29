#include "head/DataSourceManager.h"
#include "head/AppLication.h"
#include "head/MysqlManager.h"
#include "head/RedisManager.h"

DataSourceManager::~DataSourceManager() = default;
DataSourceManager::DataSourceManager(AppLication& app) :m_app(app) {
	m_sources.emplace_back(std::make_unique<MysqlManager>(m_app));
	m_sources.emplace_back(std::make_unique<RedisManager>());
};

void DataSourceManager::OnMessage(json& data) {
	if (data["data"] == "InitAll") {
		InitAll();
	}
}

void DataSourceManager::InitAll() {
	for (auto& src : m_sources) {
		src->Init();
	}
}
void DataSourceManager::StartAll() {
	for (auto& src : m_sources) {
		src->Start();
	}
}
void DataSourceManager::StopAll() {
	for (auto& src : m_sources) {
		src->Stop();
	}
}