#pragma once

namespace vektor::lib {

void init(const char *argv0);

const char *get_user_config_dir();

const char *get_user_scripts_dir();

const char *get_temp_dir();

const char *get_executable_dir();

}  // namespace vektor::lib
