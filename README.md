<p align="center">
  <img height="70" src="img/logo.png"/>  
</p>

<p align="center">
  <a href="https://en.wikipedia.org/wiki/C%2B%2B11">
    <img src="https://img.shields.io/badge/C%2B%2B-11-blue.svg" alt="standard"/>
  </a>
  <a href="https://github.com/p-ranav/tabulate/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="license"/>
  </a>
  <img src="https://img.shields.io/badge/version-1.0-blue.svg?cacheSeconds=2592000" alt="version"/>
</p>

`envy` is a small header-only library to deserialize environment variables into type-safe structs.

### Quick Start

Start by creating a `struct` where to store the values of environment variables. 

```cpp
#include <envy/envy.hpp>

struct ServerConfig {
  int  server_alive_interval = 45;
  bool compression           = true;
  int  compression_level     = 9;
  bool forward_x11           = true;
};
ENVY_STRUCT(ServerConfig, server_alive_interval, compression, compression_level, forward_x11);
```

Simply call `envy::get<T>()` to get a filled config object. For each field in the struct, `envy` will look for an environment variable with the same name in upper case, e.g., for the field named `forward_x11`, `envy` will look for an environment variable named `FORWARD_X11`.

```cpp
int main() {
  auto config = envy::get<ServerConfig>();
  
  std::cout << "Server Alive Interval : " << config.server_alive_interval << "\n";
  std::cout << "Compression Enabled?  : " << std::boolalpha << config.compression << "\n";
  std::cout << "Compression Level     : " << config.compression_level << "\n";
  std::cout << "Forward X11?          : " << std::boolalpha << config.forward_x11 << "\n"; 
}
```

Here's the stdout of the above program:

```bash
▶ ./main
Server Alive Interval : 45
Compression Enabled?  : true
Compression Level     : 9
Forward X11?          : true

▶ SERVER_ALIVE_INTERVAL=23 FORWARD_X11=0 ./main
Server Alive Interval : 23
Compression Enabled?  : true
Compression Level     : 9
Forward X11?          : false
```

### Deserialize comma-separated values

Comma-separated values can be deserialized into an `std::vector`:

```cpp
#include <envy/envy.hpp>

struct Config {
  std::vector<int> values;
};
ENVY_STRUCT(Config, values);

int main() {
  auto config = envy::get<Config>();

  for (auto& v : config.values) {
    std::cout << v << " ";
  }
}
```

```bash
▶ ./main

▶ VALUES=1,2,3,4,5 ./main
1 2 3 4 5
```

### Deserialize JSON

JSON can be deserialized into `std::map`, `std::unordered_map` or even `nlohmann::json`:

```cpp
#include <envy/envy.hpp>

struct Config {
  std::map<std::string, int> values;
};
ENVY_STRUCT(Config, values);

int main() {
  auto config = envy::get<Config>();

  for (auto& kv : config.values) {
    std::cout << kv.first << " : " << kv.second << "\n";
  }
}
```

```bash
▶ ./main

▶ VALUES='{"a": 1, "b": 2, "c": 3}' ./main
a : 1
b : 2
c : 3
```

### Application Prefix for Environment Variables

```cpp
#include <envy/envy.hpp>

struct Config {
  int   foo = 0;
  float bar = 0.0f;
  bool  baz = false;
};
ENVY_STRUCT(Config, foo, bar, baz);

int main() {
  auto config = envy::get<Config>("APP_"); // prefix for env vars

  std::cout << "foo : " << config.foo << "\n";
  std::cout << "bar : " << config.bar << "\n";
  std::cout << "baz : " << std::boolalpha << config.baz << "\n";
}
```

```bash
▶ ./main
foo : 0
bar : 0
baz : false

▶ APP_FOO=1 APP_BAR=3.14 APP_BAZ=1 ./main
foo : 1
bar : 3.14
baz : true
```

## Generating Single Header

```bash
python3 utils/amalgamate/amalgamate.py -c single_include.json -s .
```

## Contributing
Contributions are welcome, have a look at the [CONTRIBUTING.md](CONTRIBUTING.md) document for more information.

## License
The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
