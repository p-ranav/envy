
/*
  ____   _______  _____.__.
_/ __ \ /    \  \/ <   |  |  Deserialize Environment Variables
\  ___/|   |  \   / \___  |  into type-safe structs
 \___  >___|  /\_/  / ____|  https://github.com/p-ranav/envy
     \/     \/      \/     
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2020 Pranav Srinivas Kumar <pranav.srinivas.kumar@gmail.com>.
Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <cstdlib>
#include <envy/json/json.hpp>
#include <envy/visit_struct/visit_struct.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#define ENVY_STRUCT VISITABLE_STRUCT

namespace envy {

namespace details { // namespace for helper methods

template <typename Test, template <typename...> class Ref>
struct is_specialization : std::false_type {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

// Any field that is not a container type
template <typename T>
inline typename std::enable_if<
    !(details::is_specialization<T, std::vector>::value ||
      details::is_specialization<T, std::map>::value ||
      details::is_specialization<T, std::unordered_map>::value),
    T>::type
convert_to(const std::string &str) {
  std::istringstream ss(str);
  T num;
  ss >> num;
  return num;
}

// std::vector
template <typename T>
inline
    typename std::enable_if<details::is_specialization<T, std::vector>::value,
                            T>::type
    convert_to(const std::string &str) {
  T result;
  std::stringstream ss(str);

  for (typename T::value_type i; ss >> i;) {
    result.push_back(i);
    if (ss.peek() == ',')
      ss.ignore();
  }
  return result;
}

// std::map or std::unordered_map
template <typename T>
inline typename std::enable_if<
    (details::is_specialization<T, std::map>::value ||
     details::is_specialization<T, std::unordered_map>::value),
    T>::type
convert_to(const std::string &str) {
  T result;
  nlohmann::json j;

  std::stringstream ss(str);
  ss >> j;

  for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
    result.insert(std::make_pair(it.key(), it.value()));
  }
  return result;
}

struct get_env {
  const std::string prefix;

  template <typename T> void operator()(const char *name, T &value) {
    std::string field_name = name;
    std::transform(field_name.begin(), field_name.end(), field_name.begin(),
                   ::toupper);
    char *env = std::getenv((prefix + field_name).c_str());
    if (env) {
      value = convert_to<T>(env);
    }
  }
};

template <typename T> void visit(T &config, const std::string &prefix) {
  get_env get_env_visitor{.prefix = prefix};
  visit_struct::for_each(config, get_env_visitor);
}

} // namespace details

template <typename T> T get(const std::string &prefix = "") {
  T result;
  details::visit(result, prefix);
  return result;
}

} // namespace envy