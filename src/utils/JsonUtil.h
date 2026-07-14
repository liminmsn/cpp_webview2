#pragma once
#include "nlohmann/json.hpp"

using json = nlohmann::json;

inline std::string JsonToString(const json& obj)
{
    return obj.dump();
}

inline json StringToJson(const std::string& str)
{
    return json::parse(str);
}