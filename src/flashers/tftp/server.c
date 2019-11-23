/**********************************************
 * Author: ideawu(www.ideawu.net)
 * Date: 2007-04
 * File: server.c
 *********************************************/

#include "server_lib.h"
#include "tftpx.h"
#include "work_thread.h"

int main(int argc, char **argv) {
  start_tftp_server(SERVER_PORT, "./tftproot");

  return 0;
}
