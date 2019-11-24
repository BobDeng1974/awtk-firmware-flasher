#include "tkc/fs.h"
#include "stream_serial.h"
#include "ymodem/fymodem.h"

using serial::bytesize_t;
using serial::flowcontrol_t;
using serial::parity_t;
using serial::stopbits_t;

int main(int argc, char *argv[]) {
  const char* device = NULL;
  const char* filename = NULL;
  if (argc < 3) {
    printf("Usage: %s device filename\n", argv[0]);
    return 0;
  }
  device = argv[1];
  filename = argv[2];

  uint32_t baudrate = 115200;
  parity_t parity = serial::parity_odd;
  bytesize_t bytesize = serial::eightbits;
  stopbits_t stopbits = serial::stopbits_one;
  flowcontrol_t flowcontrol = serial::flowcontrol_none;
  stream_t *s = stream_serial_create(device, baudrate, bytesize, parity,
                                     stopbits, flowcontrol);
  printf("device=%s baudrate=%d bytesize=%d parity=%d stopbits=%d "
         "flowcontrol=%d\n",
         device, (int)baudrate, (int)bytesize, (int)parity, (int)stopbits,
         (int)flowcontrol);

  uint32_t size = 0;
  uint8_t* buff = (uint8_t*)file_read(filename, &size);

  if(buff != NULL) { 
    const char* msg = NULL;
    char filename[FYMODEM_FILE_NAME_MAX_LENGTH];

    memset(buff, 0x00, size);
    memset(filename, 0x00, sizeof(filename));
    if(fymodem_send(s, buff, size, filename, &msg)) {
      log_debug("send ok\n");
    } else {
      log_debug("send fail: %s\n", msg);
    }
    free(buff);
  }

  stream_close(s);

  return 0;
}
