#include "YamlWrapper.h"

YAML::Node YamlWrapper::Load(const std::string& content)
{
    return YAML::Load(content);
}

std::string YamlWrapper::Dump(const YAML::Node& node)
{
    YAML::Emitter out;
    out << node;
    return out.c_str();
}

bool YamlWrapper::HasKey(const YAML::Node& node, const std::string& key)
{
    return node[key];
}

std::string YamlWrapper::GetString(const YAML::Node& node, const std::string& key, const std::string& fallback)
{
    if (!node[key]) return fallback;
    return node[key].as<std::string>();
}

int YamlWrapper::GetInt(const YAML::Node& node, const std::string& key, int fallback)
{
    if (!node[key]) return fallback;
    return node[key].as<int>();
}

float YamlWrapper::GetFloat(const YAML::Node& node, const std::string& key, float fallback)
{
    if (!node[key]) return fallback;
    return node[key].as<float>();
}

std::vector<std::string> YamlWrapper::GetStringList(const YAML::Node& node, const std::string& key)
{
    std::vector<std::string> result;
    if (!node[key] || !node[key].IsSequence()) return result;

    for (const auto& item : node[key])
    {
        result.push_back(item.as<std::string>());
    }

    return result;
}
