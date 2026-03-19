#pragma once

#include "CONFIG_parser.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace config {

class ConfigManager {
 public:
  static ConfigManager &instance();

  bool load(const std::string &name, const std::string &path);
  bool reload(const std::string &name);
  [[nodiscard]] bool has(const std::string &name) const;

  [[nodiscard]] std::string get_string(const std::string &file,
                                       const std::string &section,
                                       const std::string &key,
                                       const std::string &default_val = "") const;

  [[nodiscard]] int get_int(const std::string &file,
                            const std::string &section,
                            const std::string &key,
                            int default_val = 0) const;

  [[nodiscard]] float get_float(const std::string &file,
                                const std::string &section,
                                const std::string &key,
                                float default_val = 0.0f) const;

  [[nodiscard]] bool get_bool(const std::string &file,
                              const std::string &section,
                              const std::string &key,
                              bool default_val = false) const;

  [[nodiscard]] std::unordered_map<std::string, std::string> get_section(
      const std::string &file, const std::string &section) const;

  ConfigManager &operator=(const ConfigManager &) = delete;
  explicit ConfigManager(const ConfigManager &) = delete;

 private:
  ConfigManager() = default;
  ~ConfigManager() = default;

  struct ConfigFileData {
    std::string path;
    std::unique_ptr<ConfigParser> parser;
  };

  std::unordered_map<std::string, ConfigFileData> configs_;
};

}  // namespace config
