
#pragma once
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <visit_struct/visit_struct.hpp>

#define ENVY_CONFIG VISITABLE_STRUCT

namespace envy {

template <typename T> T convert_to(const std::string &str) {
  std::istringstream ss(str);
  T num;
  ss >> num;
  return num;
}

struct get_env {
  template <typename T> void operator()(const char *name, T &value) {
    std::string field_name = name;
    std::transform(field_name.begin(), field_name.end(), field_name.begin(),
                   ::toupper);
    char * env = std::getenv(field_name.c_str());
    if (env) {
      value = convert_to<T>(env);
    }
  }
};

template <typename T> void visit(T &config) {
  visit_struct::for_each(config, get_env{});
}

template <typename T> T get_env() {
  T result;
  visit(result);
  return result;
}

} // namespace envy