/**********************************************
 * Author: ideawu(www.ideawu.net)
 * Date: 2007-06
 * File: client.c
 *********************************************/

#include "client.h"

extern void socketInit();
extern void socketDeinit();

int sock;
socklen_t addr_len;
struct sockaddr_in server;
int blocksize = DATA_SIZE;

int main(int argc, char **argv) {
  char *filename;
  char *server_ip;
  unsigned short port = SERVER_PORT;

  socketInit();
  addr_len = sizeof(struct sockaddr_in);

  if (argc <= 3) {
    printf("Usage: %s server_ip port filename\n", argv[0]);
    return 0;
  }

  server_ip = argv[1];
  port = (unsigned short)atoi(argv[2]);
  filename = argv[3];

  printf("Connect to server at %s:%d", server_ip, port);

  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    printf("Server socket could not be created.\n");
    return 0;
  }

  // Initialize server address
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
#ifndef WIN32
  inet_pton(AF_INET, server_ip, &(server.sin_addr.s_addr));
#else
  InetPton(AF_INET, server_ip, &(server.sin_addr.s_addr));
#endif

  do_get(filename, filename);

  socketDeinit();

  return 0;
}

// Download a file from the server.
void do_get(char *remote_file, char *local_file) {
  struct tftpx_packet snd_packet, rcv_packet;
  struct sockaddr_in sender;

  int r_size = 0;
  int time_wait_data;
  ushort block = 1;

  // Send request.
  snd_packet.cmd = htons(CMD_RRQ);
  char *data = snd_packet.filename;
  sprintf(data, "%s%c%s%c%d%c", remote_file, 0, "octet", 0, blocksize, 0);
  sendto(sock, (const char*)&snd_packet, sizeof(struct tftpx_packet), 0,
         (struct sockaddr *)&server, addr_len);

  FILE *fp = fopen(local_file, "w");
  if (fp == NULL) {
    printf("Create file \"%s\" error.\n", local_file);
    return;
  }

  // Receive data.
  snd_packet.cmd = htons(CMD_ACK);
  do {
    for (time_wait_data = 0; time_wait_data < PKT_RCV_TIMEOUT * PKT_MAX_RXMT;
         time_wait_data += 10000) {
      // Try receive(Nonblock receive).
      r_size = recvfrom(sock, (char*)&rcv_packet, sizeof(struct tftpx_packet),
                        MSG_DONTWAIT, (struct sockaddr *)&sender, &addr_len);
      if (r_size > 0 && r_size < 4) {
        printf("Bad packet: r_size=%d\n", r_size);
      }
      if (r_size >= 4 && rcv_packet.cmd == htons(CMD_DATA) &&
          rcv_packet.block == htons(block)) {
        printf("DATA: block=%d, data_size=%d\n", ntohs(rcv_packet.block),
               r_size - 4);
        // Send ACK.
        snd_packet.block = rcv_packet.block;
        sendto(sock, (char*)&snd_packet, sizeof(struct tftpx_packet), 0,
               (struct sockaddr *)&sender, addr_len);
        fwrite(rcv_packet.data, 1, r_size - 4, fp);
        break;
      }
      usleep(10000);
    }
    if (time_wait_data >= PKT_RCV_TIMEOUT * PKT_MAX_RXMT) {
      printf("Wait for DATA #%d timeout.\n", block);
      goto do_get_error;
    }
    block++;
  } while (r_size == blocksize + 4);
  // printf("\nReceived %d bytes.\n", total_bytes);

do_get_error:
  fclose(fp);
}

