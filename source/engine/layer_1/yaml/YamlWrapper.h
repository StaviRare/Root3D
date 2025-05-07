#pragma once

#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>

class YamlWrapper
{
    public:
    static YAML::Node Load(const std::string& content);
    static std::string Dump(const YAML::Node& node);

    static bool HasKey(const YAML::Node& node, const std::string& key);
    static std::string GetString(const YAML::Node& node, const std::string& key, const std::string& fallback = "");
    static int GetInt(const YAML::Node& node, const std::string& key, int fallback = 0);
    static float GetFloat(const YAML::Node& node, const std::string& key, float fallback = 0.0f);
    static std::vector<std::string> GetStringList(const YAML::Node& node, const std::string& key);
};
