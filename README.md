## envy

`envy` is a small header-only library to deserialize environment variables into type-safe structs.

### Getting Started

Start by creating a `struct` where to store the values of environment variables. 

```cpp
#include <envy/envy.hpp>

struct ServerConfig {
  int server_alive_interval = 45;
  bool compression          = true;
  int compression_level     = 9;
  bool forward_x11          = true;
};
ENVY_CONFIG(ServerConfig, server_alive_interval, compression, compression_level, forward_x11);
```

Simply call `envy::get_env<T>()` to get a filled config object. For each field in the struct, `envy` will look for an environment variable with the same name in upper case, e.g., for the field named `forward_x11`, `envy` will look for an environment variable named `FORWARD_X11`.

```cpp
int main() {

  // Get environment variables and save into a `ServerConfig` object
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
