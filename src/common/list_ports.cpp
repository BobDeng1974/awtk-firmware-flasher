#include "tkc/str.h"
#include "serial/serial.h"

extern "C" ret_t serial_ports_list(str_t* str) {
  std::vector<serial::PortInfo> ports = serial::list_ports();

  for(uint32_t i = 0; i < ports.size(); i++) {
    str_append(str, ports[i].port.c_str());
    str_append(str, ";");
  }

  return RET_OK;
}
