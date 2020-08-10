
#pragma once
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <visit_struct/visit_struct.hpp>

#define ENVY_CONFIG VISITABLE_STRUCT

namespace envy {

namespace details { // namespace for helper methods

template <typename... Ts> struct is_container_helper {};

template <typename T, typename _ = void>
struct is_container : std::false_type {};

template <> struct is_container<std::string> : std::false_type {};

template <typename T>
struct is_container<
    T,
    std::conditional_t<false,
                       is_container_helper<typename T::value_type,
                                           decltype(std::declval<T>().begin()),
                                           decltype(std::declval<T>().end()),
                                           decltype(std::declval<T>().size())>,
                       void>> : std::true_type {};

template <typename T>
static constexpr bool is_container_v = is_container<T>::value;

}

template <typename T> 
inline typename std::enable_if<!details::is_container<T>::value, T>::type
convert_to(const std::string &str) {
  std::istringstream ss(str);
  T num;
  ss >> num;
  return num;
}

template <typename T>
inline typename std::enable_if<details::is_container<T>::value, T>::type
convert_to(const std::string & str) {
  T result;
  std::stringstream ss(str);

  for (typename T::value_type i; ss >> i;) {
    result.push_back(i);    
      if (ss.peek() == ',')
        ss.ignore();
    }
  return result;
}

struct get_env {
  const std::string prefix;

  template <typename T> void operator()(const char *name, T &value) {
    std::string field_name = name;
    std::transform(field_name.begin(), field_name.end(), field_name.begin(),
                   ::toupper);
    char * env = std::getenv((prefix + field_name).c_str());
    if (env) {
      value = convert_to<T>(env);
    }
  }
};

template <typename T> void visit(T &config, const std::string & prefix) {
  get_env get_env_visitor {.prefix = prefix};
  visit_struct::for_each(config, get_env_visitor);
}

template <typename T> T get_env(const std::string & prefix = "") {
  T result;
  visit(result, prefix);
  return result;
}

struct debug_printer {
  template <typename T>
  void operator()(const char * name, const T & value) {
    std::cerr << name << ": " << value << std::endl;
  }
};

template <typename T>
void print(const T & config) {
  visit_struct::for_each(config, debug_printer{});
}

} // namespace envy