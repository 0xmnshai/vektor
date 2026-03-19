#include "../CONFIG_parser.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <utility>

namespace config {

ConfigParser::ConfigParser(const std::string &filename) : filename_(filename) {}

bool ConfigParser::load()
{
  std::ifstream file(filename_);
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  std::string current_section = "default";

  while (std::getline(file, line)) {
    line = trim(line);
    if (line.empty() || line[0] == ';' || line[0] == '#') {
      continue;
    }

    if (line.front() == '[' && line.back() == ']') {
      current_section = trim(line.substr(1, line.size() - 2));
      continue;
    }

    size_t eq = line.find('=');
    if (eq != std::string::npos) {
      std::string key = trim(line.substr(0, eq));
      std::string value = trim(line.substr(eq + 1));
      data_[current_section][key] = value;
    }
  }
  return true;
}

void ConfigParser::save()
{
  std::ofstream file(filename_);
  if (!file.is_open())
    return;

  for (const auto &[section, keys] : data_) {
    file << "[" << section << "]\n";
    for (const auto &[key, value] : keys) {
      file << key << "=" << value << "\n";
    }
    file << "\n";
  }
}

std::string ConfigParser::get_string(const std::string &section,
                                     const std::string &key,
                                     const std::string &default_value) const
{
  auto sec_it = data_.find(section);
  if (sec_it != data_.end()) {
    auto key_it = sec_it->second.find(key);
    if (key_it != sec_it->second.end()) {
      return key_it->second;
    }
  }
  return default_value;
}

int ConfigParser::get_int(const std::string &section,
                          const std::string &key,
                          int default_value) const
{
  std::string val = get_string(section, key);
  if (val.empty())
    return default_value;
  try {
    return std::stoi(val);
  }
  catch (...) {
    return default_value;
  }
}

float ConfigParser::get_float(const std::string &section,
                              const std::string &key,
                              float default_value) const
{
  std::string val = get_string(section, key);
  if (val.empty())
    return default_value;
  try {
    return std::stof(val);
  }
  catch (...) {
    return default_value;
  }
}

bool ConfigParser::get_bool(const std::string &section,
                            const std::string &key,
                            bool default_value) const
{
  std::string val = to_lower(get_string(section, key));
  if (val == "true" || val == "1" || val == "yes")
    return true;
  if (val == "false" || val == "0" || val == "no")
    return false;
  return default_value;
}

std::unordered_map<std::string, std::string> ConfigParser::get_section(const std::string &section) const {
  auto sec_it = data_.find(section);
  if (sec_it != data_.end()) {
    return sec_it->second;
  }
  return {};
}

void ConfigParser::set_string(const std::string &section,
                              const std::string &key,
                              const std::string &value)
{
  data_[section][key] = value;
}

void ConfigParser::set_int(const std::string &section, const std::string &key, int value)
{
  data_[section][key] = std::to_string(value);
}

void ConfigParser::set_float(const std::string &section, const std::string &key, float value)
{
  data_[section][key] = std::to_string(value);
}

void ConfigParser::set_bool(const std::string &section, const std::string &key, bool value)
{
  data_[section][key] = value ? "true" : "false";
}

std::string ConfigParser::trim(const std::string &str)
{
  size_t start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";
  size_t end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}

std::string ConfigParser::to_lower(const std::string &str)
{
  std::string lower_str = str;
  std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c) {
    return std::tolower(c);
  });
  return lower_str;
}

}  // namespace config
