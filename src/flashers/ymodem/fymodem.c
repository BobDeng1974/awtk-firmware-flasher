/**
 * Free YModem implementation.
 *
 * Fredrik Hederstierna 2014
 *
 * This file is in the public domain.
 * You can do whatever you want with it.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "fymodem.h"
#include "err_msg.h"
#include "progress.h"

#define _sleep(s) msleep(s * 100)
// filesize 999999999999999 should be enough...
#define FILE_SIZE_LENGTH (16)

#define PACKET_SEQNO_INDEX (1)
#define PACKET_SEQNO_COMP_INDEX (2)
#define PACKET_HEADER (3)  // start, block, block-complement
#define PACKET_TRAILER (2) // CRC bytes
#define PACKET_OVERHEAD (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE (128)
#define PACKET_1K_SIZE (1024)
#define PACKET_RX_TIMEOUT_SEC (1)
#define PACKET_ERROR_MAX_NBR (50)

#define ABT1 (0x41) // 'A' == 0x41, abort by user
#define ABT2 (0x61) // 'a' == 0x61, abort by user
#define SOH (0x01)  // start of 128-byte data packet
#define STX (0x02)  // start of 1024-byte data packet
#define EOT (0x04)  // End Of Transmission
#define ACK (0x06)  // ACKnowledge, receive OK
#define NAK (0x15)  // Negative ACKnowledge, receiver ERROR; retry
#define CAN (0x18)  // two CAN in succession will abort transfer
#define CRC                                                                    \
  (0x43) // 'C' == 0x43, request 16-bit CRC;
         // use in place of first NAK for CRC mode

//------------------------------------------------

static int do_get_char(stream_t *stream, int tms_s) {
  _sleep(tms_s);
  int ret = stream_read_c(stream);
  //	printf("getchar: c=0x%02x ret=%d\n", ret, ret);

  return ret;
}

static int do_put_char(stream_t *stream, uint8_t c) {
  int ret = stream_write_c(stream, c);
  //	printf("putchar: c=0x%02x ret=%d\n", (int)c, ret);

  return ret;
}

static void do_flush(stream_t *stream) { stream_flush(stream); }

#define _getchar(tmo_s) do_get_char(stream, tmo_s)
#define _putchar(c) do_put_char(stream, c)
#define _flush(stream) do_flush(stream)

// error log function
#define ERR(fmt, ...) printf(fmt, __VA_ARGS__)

//------------------------------------------------
// calculate crc16-ccitt very fast
// Idea from: http://www.ccsinfo.com/forum/viewtopic.php?t=24977
static uint16_t ym_crc16(const uint8_t *buf, uint16_t len) {
  uint16_t x;
  uint16_t crc = 0;
  while (len--) {
    x = (crc >> 8) ^ *buf++;
    x ^= x >> 4;
    crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
  }
  return crc;
}

//-------------------------------------------------
// write 32bit value as asc to buffer, return chars written.
static uint32_t ym_writeU32(uint32_t val, uint8_t *buf) {
  uint32_t ci = 0;
  if (val == 0) {
    // If already zero then just return zero
    buf[ci++] = '0';
  } else {
    // Maximum number of decimal digits in uint32_t is 10
    uint8_t s[11];
    int32_t i = sizeof(s) - 1;
    s[i] = 0;
    while ((val > 0) && (i > 0)) {
      s[--i] = (val % 10) + '0';
      val /= 10;
    }
    uint8_t *sp = &s[i];
    while (*sp) {
      buf[ci++] = *sp++;
    }
  }
  buf[ci] = 0;
  return ci;
}

//-------------------------------------------------
// read 32bit asc value from buffer
static void ym_readU32(const uint8_t *buf, uint32_t *val) {
  const uint8_t *s = buf;
  uint32_t res = 0;
  uint8_t c;
  // Trim & strip leading spaces if any
  do {
    c = *s++;
  } while (c == ' ');
  while ((c >= '0') && (c <= '9')) {
    c -= '0';
    res *= 10;
    res += c;
    c = *s++;
  }
  *val = res;
}

//--------------------------------------------------
/**
 * Receive a packet from sender
 * @param rxlen
 *     0: end of transmission
 *    -1: abort by sender
 *    >0: packet length
 * @return 0: normally return, success
 *        -1: timeout or packet error
 *         1: abort by user / corrupt packet
 */
static int32_t ym_rx_packet(stream_t *stream, uint8_t *rxdata, int32_t *rxlen,
                            uint32_t packets_rxed, uint32_t timeout_sec) {
  (void)timeout_sec;
  *rxlen = 0;

  int32_t c = _getchar(timeout_sec);
  if (c < 0) {
    // end of stream
    return -1;
  }

  uint32_t rx_packet_size;

  switch (c) {
  case SOH:
    rx_packet_size = PACKET_SIZE;
    break;
  case STX:
    rx_packet_size = PACKET_1K_SIZE;
    break;
  case EOT:
    // ok
    return 0;
  case CAN:
    c = _getchar(timeout_sec);
    if (c == CAN) {
      *rxlen = -1;
      // ok
      return 0;
    }
  case CRC:
    if (packets_rxed == 0) {
      // could be start condition, first byte
      printf("error: packets_rxed == 0\n");
      return 1;
    }
  case ABT1:
  case ABT2:
    // User try abort, 'A' or 'a' received
    printf("error: User try abort, 'A' or 'a' received\n");
    return 1;
  default:
    // This case could be the result of corruption on the first octet
    // of the packet, but it's more likely that it's the user banging
    // on the terminal trying to abort a transfer. Technically, the
    // former case deserves a NAK, but for now we'll just treat this
    // as an abort case.
    *rxlen = -1;
    return 0;
  }

  // store data rxed
  *rxdata = (uint8_t)c;

  uint32_t i;
  for (i = 1; i < (rx_packet_size + PACKET_OVERHEAD); i++) {
    c = _getchar(timeout_sec);
    if (c < 0) {
      // end of stream
      return -1;
    }
    // store data rxed
    rxdata[i] = (uint8_t)c;
  }

  // Just a sanity check on the sequence number/complement value.
  // Caller should check for in-order arrival.
  uint8_t seq_nbr = (rxdata[PACKET_SEQNO_INDEX] & 0xff);
  uint8_t seq_cmp = ((rxdata[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff);
  if (seq_nbr != seq_cmp) {
    // seq nbr error
    return 1;
  }

  // Check CRC16 match
  uint16_t crc16_val = ym_crc16((const unsigned char *)(rxdata + PACKET_HEADER),
                                rx_packet_size + PACKET_TRAILER);
  if (crc16_val) {
    // CRC error, non zero
    return 1;
  }
  *rxlen = rx_packet_size;
  // success
  return 0;
}

//-------------------------------------------------
/**
 * Receive a file using the ymodem protocol
 * @param rxdata Pointer to the first byte
 * @param rxlen  Max in length
 * @return The length of the file received, or 0 on error
 */
int32_t fymodem_receive(stream_t *stream, uint8_t *rxdata, size_t rxlen,
                        char filename[FYMODEM_FILE_NAME_MAX_LENGTH]) {
  // alloc 1k on stack, ok?
  uint8_t rx_packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
  int32_t rx_packet_len;

  uint8_t filesize_asc[FILE_SIZE_LENGTH];
  uint32_t filesize = 0;

  bool first_try = true;
  bool session_done = false;

  uint32_t nbr_errors = 0;

  // z-term string
  filename[0] = 0;

  // receive files
  do {
    if (!first_try) {
      _putchar(CRC);
    }
    first_try = false;

    bool crc_nak = true;
    bool file_done = false;
    uint32_t packets_rxed = 0;

    // set start position of rxing data
    uint8_t *rxptr = rxdata;
    do {
      // receive packets
      int32_t res = ym_rx_packet(stream, rx_packet_data, &rx_packet_len,
                                 packets_rxed, PACKET_RX_TIMEOUT_SEC);
      switch (res) {
      case 0: {
        nbr_errors = 0;
        switch (rx_packet_len) {
        case -1: {
          // aborted by sender
          _putchar(ACK);
          return 0;
        }
        case 0: {
          // EOT - End Of Transmission
          _putchar(ACK);
          // Should add some sort of sanity check on the number of
          // packets received and the advertised file length.
          file_done = true;
          break;
        }
        default: {
          // normal packet - check seq nbr
          uint8_t seq_nbr = rx_packet_data[PACKET_SEQNO_INDEX];
          if (seq_nbr != (packets_rxed & 0xff)) {
            // wrong seq number
            _putchar(NAK);
          } else {
            if (packets_rxed == 0) {
              // The spec suggests that the whole data section should
              // be zeroed, but some senders might not do this. If
              // we have a NULL filename and the first few digits of
              // the file length are zero, we'll call it empty.
              int32_t i;
              for (i = PACKET_HEADER; i < PACKET_HEADER + 4; i++) {
                if (rx_packet_data[i] != 0) {
                  break;
                }
              }
              // non-zero bytes found in header, filename packet has data
              if (i < PACKET_HEADER + 4) {
                // read file name
                uint8_t *file_ptr = (uint8_t *)(rx_packet_data + PACKET_HEADER);
                i = 0;
                while (*file_ptr && (i < FYMODEM_FILE_NAME_MAX_LENGTH)) {
                  filename[i++] = *file_ptr++;
                }
                filename[i++] = '\0';
                file_ptr++;
                // read file size
                i = 0;
                while ((*file_ptr != ' ') && (i < FILE_SIZE_LENGTH)) {
                  filesize_asc[i++] = *file_ptr++;
                }
                filesize_asc[i++] = '\0';
                // convert file size
                ym_readU32(filesize_asc, &filesize);
                // check file size
                if (filesize > rxlen) {
                  ERR("rx buffer too small (0x%08x vs 0x%08x)\n",
                      (int32_t)rxlen, filesize);
                  goto rx_err_handler;
                }
                _putchar(ACK);
                _putchar(crc_nak ? CRC : NAK);
                crc_nak = false;
              } else {
                // filename packet is empty; end session
                _putchar(ACK);
                file_done = true;
                session_done = true;
                break;
              }
            } else {
              // This shouldn't happen, but we check anyway in case the
              // sender sent wrong info in its filename packet
              if (((rxptr + rx_packet_len) - rxdata) > (int32_t)rxlen) {
                ERR("rx buffer overflow (exceeded 0x%08x)\n", (int32_t)rxlen);
                goto rx_err_handler;
              }
              int32_t i;
              for (i = 0; i < rx_packet_len; i++) {
                rxptr[i] = rx_packet_data[PACKET_HEADER + i];
              }
              rxptr += rx_packet_len;
              _putchar(ACK);
            }
            packets_rxed++;
          } // sequence number check ok
        }   // default
        }   // inner switch
        break;
      } // case 0
      default: {
        if (packets_rxed > 0) {
          nbr_errors++;
          if (nbr_errors >= PACKET_ERROR_MAX_NBR) {
            ERR("rx errors too many: %d - ABORT.\n", nbr_errors);
            goto rx_err_handler;
          }
        }
        _putchar(CRC);
        break;
      } // default
      } // switch

      // check end of receive packets
    } while (!file_done);

    // check end of receive files
  } while (!session_done);

  // return bytes received
  return filesize;

rx_err_handler:
  _putchar(CAN);
  _putchar(CAN);
  _sleep(1);
  return 0;
}

//------------------------------------
static void ym_send_packet(stream_t *stream, uint8_t *txdata,
                           int32_t block_nbr) {
  int32_t tx_packet_size;

  // We use a short packet for block 0 - all others are 1K
  if (block_nbr == 0) {
    tx_packet_size = PACKET_SIZE;
  } else {
    tx_packet_size = PACKET_1K_SIZE;
  }

  uint16_t crc16_val = ym_crc16(txdata, tx_packet_size);

  // 128 byte packets use SOH, 1K use STX
  _putchar((block_nbr == 0) ? SOH : STX);
  // write seq numbers
  _putchar(block_nbr & 0xFF);
  _putchar(~block_nbr & 0xFF);

  int ret = stream_write(stream, txdata, tx_packet_size);
  printf("stream_write(%d) = %d\n", tx_packet_size, ret);
  // write crc16
  _putchar((crc16_val >> 8) & 0xFF);
  _putchar(crc16_val & 0xFF);
}

//-----------------------------------------------
// Send block 0 (the filename block). filename might be truncated to fit.
static void ym_send_packet0(stream_t *stream, const char *filename,
                            int32_t filesize) {
  int32_t pos = 0;
  // put 256byte on stack, ok? reuse other stack mem?
  uint8_t block[PACKET_SIZE];
  if (filename) {
    // write filename
    while (*filename && (pos < PACKET_SIZE - FILE_SIZE_LENGTH - 2)) {
      block[pos++] = *filename++;
    }
    // z-term filename
    block[pos++] = 0;

    // write size, TODO: check if buffer can overwritten here.
    pos += ym_writeU32(filesize, &block[pos]);
  }

  // z-terminate string, pad with zeros
  while (pos < PACKET_SIZE) {
    block[pos++] = 0;
  }

  // send header block
  ym_send_packet(stream, block, 0);
}

//-------------------------------------------------
static bool_t ym_send_data_packets(stream_t *stream, uint8_t *txdata,
                                   uint32_t txlen, uint32_t timeout_sec) {
  (void)timeout_sec;
  int32_t block_nbr = 1;
  uint32_t total = txlen;
  uint32_t finish = 0;

  while (txlen > 0) {
    // check if send full 1k packet
    uint32_t send_size;
    if (txlen > PACKET_1K_SIZE) {
      send_size = PACKET_1K_SIZE;
    } else {
      send_size = txlen;
    }
    // send packet
    ym_send_packet(stream, txdata, block_nbr);
    int32_t c = _getchar(timeout_sec);
    switch (c) {
    case ACK: {
      txdata += send_size;
      txlen -= send_size;
      block_nbr++;
      finish += send_size;
      progress_notify("ymodem", finish, total);
      break;
    }
    case -1:
    case CAN: {
      return FALSE;
    }
    default:
      break;
    }
  }

  int32_t ch;
  do {
    _putchar(EOT);
    ch = _getchar(timeout_sec);
  } while ((ch != ACK) && (ch != -1));

  // Send last data packet
  if (ch == ACK) {
    ch = _getchar(timeout_sec);
    if (ch == CRC) {
      do {
        ym_send_packet0(stream, 0, 0);
        ch = _getchar(timeout_sec);
      } while ((ch != ACK) && (ch != -1));
    }
  }

  return TRUE;
}

//-------------------------------------------------------
bool_t fymodem_send(stream_t *stream, uint8_t *txdata, size_t txsize,
                    const char *filename, const char **err_msg) {
  // Flush the RX FIFO, after a cool off delay
  _sleep(1);
  _flush(stream);

  int retry_times = 100;
  printf("send: %s %zu\n", filename, txsize);
  // Not in the specs, just for balance
  int32_t ch;
  do {
    retry_times--;
    printf("putchar ch=CRC\n");
    _putchar(CRC);
    ch = _getchar(1);
    printf("c=0x%02x(%c)\n", ch, ch);
  } while ((ch < 0 || ch != CRC) && retry_times > 0);

  // We require transfer with CRC
  if (ch != CRC) {
    printf("fymodem_send: not get CRC(0x%02x)\n", ch);
    SET_ERR_MSG(err_msg, MSG_SYNC_PEER_FAILED);
    goto tx_err_handler;
  }
  printf("fymodem_send: get CRC(0x%02x)\n", ch);

  retry_times = 10;
  bool crc_nak = true;
  bool file_done = false;
  do {
    ym_send_packet0(stream, filename, (int32_t)txsize);
    // When the receiving program receives this block and successfully
    // opened the output file, it shall acknowledge this block with an ACK
    // character and then proceed with a normal XMODEM file transfer
    // beginning with a "C" or NAK tranmsitted by the receiver.
    ch = _getchar(PACKET_RX_TIMEOUT_SEC);
    if (ch == ACK) {
      printf("fymodem_send: send header ok.\n");
      ch = _getchar(PACKET_RX_TIMEOUT_SEC);
      if (ch == CRC) {
        printf("fymodem_send: start to send file.\n");
        if (ym_send_data_packets(stream, txdata, (uint32_t)txsize,
                                 PACKET_RX_TIMEOUT_SEC)) {
          file_done = true;
        } else {
          SET_ERR_MSG(err_msg, MSG_SEND_FILE_FAILED);
          goto tx_err_handler;
        }
      } else {
        SET_ERR_MSG(err_msg, MSG_GET_HEADER_CRC_FAILED);
        printf("fymodem_send: get crc error(0x%02x), abort send file.\n", ch);
        goto tx_err_handler;
      }
    } else if ((ch == CRC) && (crc_nak)) {
      crc_nak = false;
      retry_times--;
      continue;
    } else if ((ch != NAK) && retry_times <= 0) {
      printf("fymodem_send: send header fail.\n");
      SET_ERR_MSG(err_msg, MSG_SEND_FILE_HEAD_FAILED);
      goto tx_err_handler;
    } else {
      retry_times--;
    }
  } while (!file_done);

  return TRUE;

tx_err_handler:
  _putchar(CAN);
  _putchar(CAN);
  _sleep(1);

  return FALSE;
}
