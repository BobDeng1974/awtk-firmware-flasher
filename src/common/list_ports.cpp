#include "tkc/str.h"
#include "serial/serial.h"

extern "C" ret_t serial_ports_list(str_t* str) {
  std::vector<serial::PortInfo> ports = serial::list_ports();

  for(uint32_t i = 0; i < ports.size(); i++) {
    str_append(str, ports[i].port.c_str());
    str_append(str, ";");
  }
#if defined(MACOS)
  str_append(str, "/dev/ttys008;");
  str_append(str, "/dev/ttys009;");
#elif defined(LINUX)  
  str_append(str, "/dev/pts/4;");
  str_append(str, "/dev/pts/5;");
#endif/*WIN32*/

  return RET_OK;
}
