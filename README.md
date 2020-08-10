## envy

`envy` is a small header-only library to deserialize environment variables into type-safe structs.

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

int main() {

  auto config = envy::get_env<ServerConfig>();
  
  std::cout << "Server Alive Interval : " << config.server_alive_interval << "\n";
  std::cout << "Compression Enabled?  : " << std::boolalpha << config.compression << "\n";
  std::cout << "Compression Level     : " << config.compression_level << "\n";
  std::cout << "Forward X11?          : " << std::boolalpha << config.forward_x11 << "\n"; 

}
```

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