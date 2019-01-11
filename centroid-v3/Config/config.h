// In config.h

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace config
{
extern int some_config_int;
extern std::string some_config_string;

bool load_config_file();
} // namespace config

#endif