#include "tkc/fs.h"
#include "stream_serial.h"
#include "ymodem/fymodem.h"

using serial::bytesize_t;
using serial::flowcontrol_t;
using serial::parity_t;
using serial::stopbits_t;

int main(int argc, char *argv[]) {
  const char* device = NULL;
  if (argc < 2) {
    printf("Usage: %s device\n", argv[0]);
    return 0;
  }
  device = argv[1];

  uint32_t baudrate = 115200;
  parity_t parity = serial::parity_none;
  bytesize_t bytesize = serial::eightbits;
  stopbits_t stopbits = serial::stopbits_one;
  flowcontrol_t flowcontrol = serial::flowcontrol_none;
  stream_t *s = stream_serial_create(device, baudrate, bytesize, parity,
                                     stopbits, flowcontrol);
  printf("device=%s baudrate=%d bytesize=%d parity=%d stopbits=%d "
         "flowcontrol=%d\n",
         device, (int)baudrate, (int)bytesize, (int)parity, (int)stopbits,
         (int)flowcontrol);

  uint32_t size = 1024 * 1024 * 8;
  uint8_t* buff = (uint8_t*)malloc(size);
  if(buff != NULL) { 
    int32_t ret = 0;
    char filename[FYMODEM_FILE_NAME_MAX_LENGTH];

    memset(buff, 0x00, size);
    memset(filename, 0x00, sizeof(filename));
    ret = fymodem_receive(s, buff, size, filename);
    if(ret > 0) {
      file_write(filename, buff, ret);
      log_debug("write to %s\n", filename);
    } else {
      log_debug("receive failed\n");
    }
    free(buff);
  }

  stream_close(s);

  return 0;
}
