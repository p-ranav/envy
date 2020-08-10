## envy

`envy` is a small header-only library to deserialize environment variables into type-safe structs.

### Getting Started

Start by creating a `struct` where to store the values of environment variables. `envy` will check for environment variables that match the name of the fields in this struct. Specifically, `envy` will match the upper-case version of the field names, e.g., a field named `foo_bar` will be matched with an environment variable named `FOO_BAR`. 

```cpp
#include <envy/envy.hpp>

struct ServerConfig {
  // default config
  std::size_t server_alive_interval = 45;
  bool compression                  = true;
  std::size_t compression_level     = 9;
  bool forward_x11                  = true;
};
ENVY_CONFIG(ServerConfig, server_alive_interval, compression, compression_level, forward_x11);
```

Now, simply call `envy::get_env<T>()` to get a filled config object.

```cpp
int main() {

  auto config = envy::get_env<ServerConfig>();
  
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
