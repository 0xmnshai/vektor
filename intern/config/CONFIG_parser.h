#pragma once

#include <string>
#include <unordered_map>

namespace config {

class ConfigParser {
 public:
  explicit ConfigParser(const std::string &filename);

  bool load();
  void save();

  [[nodiscard]] std::string get_string(const std::string &section,
                                       const std::string &key,
                                       const std::string &default_value = "") const;
  [[nodiscard]] int get_int(const std::string &section,
                            const std::string &key,
                            int default_value = 0) const;
  [[nodiscard]] float get_float(const std::string &section,
                                const std::string &key,
                                float default_value = 0.0f) const;
  [[nodiscard]] bool get_bool(const std::string &section,
                              const std::string &key,
                              bool default_value = false) const;
                              
  [[nodiscard]] std::unordered_map<std::string, std::string> get_section(const std::string &section) const;

  void set_string(const std::string &section, const std::string &key, const std::string &value);
  void set_int(const std::string &section, const std::string &key, int value);
  void set_float(const std::string &section, const std::string &key, float value);
  void set_bool(const std::string &section, const std::string &key, bool value);

 private:
  std::string filename_;
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data_;

  [[nodiscard]] static std::string trim(const std::string &str);
  [[nodiscard]] static std::string to_lower(const std::string &str);
};

}  // namespace config
