#include "../CONFIG_manager.h"

namespace config {

ConfigManager &ConfigManager::instance()
{
  static ConfigManager instance;
  return instance;
}

bool ConfigManager::load(const std::string &name, const std::string &path)
{
  auto parser = std::make_unique<ConfigParser>(path);
  if (!parser->load()) {
    return false;
  }

  ConfigFileData cfg;
  cfg.path = path;
  cfg.parser = std::move(parser);
  configs_[name] = std::move(cfg);
  return true;
}

bool ConfigManager::reload(const std::string &name)
{
  auto it = configs_.find(name);
  if (it == configs_.end()) {
    return false;
  }
  return it->second.parser->load();
}

bool ConfigManager::has(const std::string &name) const
{
  return configs_.find(name) != configs_.end();
}

std::string ConfigManager::get_string(const std::string &file,
                                      const std::string &section,
                                      const std::string &key,
                                      const std::string &default_val) const
{
  auto it = configs_.find(file);
  if (it == configs_.end())
    return default_val;
  return it->second.parser->get_string(section, key, default_val);
}

int ConfigManager::get_int(const std::string &file,
                           const std::string &section,
                           const std::string &key,
                           int default_val) const
{
  auto it = configs_.find(file);
  if (it == configs_.end())
    return default_val;
  return it->second.parser->get_int(section, key, default_val);
}

float ConfigManager::get_float(const std::string &file,
                               const std::string &section,
                               const std::string &key,
                               float default_val) const
{
  auto it = configs_.find(file);
  if (it == configs_.end())
    return default_val;
  return it->second.parser->get_float(section, key, default_val);
}

bool ConfigManager::get_bool(const std::string &file,
                             const std::string &section,
                             const std::string &key,
                             bool default_val) const
{
  auto it = configs_.find(file);
  if (it == configs_.end())
    return default_val;
  return it->second.parser->get_bool(section, key, default_val);
}

std::unordered_map<std::string, std::string> ConfigManager::get_section(
    const std::string &file, const std::string &section) const
{
  auto it = configs_.find(file);
  if (it == configs_.end())
    return {};
  return it->second.parser->get_section(section);
}

}  // namespace config
