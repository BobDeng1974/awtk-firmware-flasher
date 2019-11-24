/**
 * File:   firmware_flasher.c
 * Author: AWTK Develop Team
 * Brief:  firmware_flasher
 *
 * Copyright (c) 2018 - 2019  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2019-11-23 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/fs.h"
#include "tkc/utils.h"
#include "firmware_flasher.h"
#include "transferer_tcp.h"
#include "transferer_serial.h"
#include "transferer_factory.h"
#include "transferer_tftp_server.h"

static firmware_flasher_t s_flasher;

#ifdef WIN32
#include <Windows.h>
char *toLocaleString(const char *str) {
  WCHAR *strSrc = NULL;
  char *szRes = NULL;

  int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
  strSrc = (WCHAR *)calloc(len + 1, sizeof(WCHAR));
  MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, len);
  len = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
  szRes = (char *)calloc(len + 1, 1);
  WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, len, NULL, NULL);
  free(strSrc);

  printf("%s => %s\n", str, szRes);
  return szRes;
}
#else
char *toLocaleString(char *str) { return str; }
#endif

static ret_t firmware_flasher_gen_url(firmware_flasher_t* flasher, str_t* str) {
  char arg[256];
  if(flasher->flash_type == FLASH_TYPE_TFTP) {
    str_append(str, "tftp_server://localhost/");
    str_append(str, flasher->filename.str);
    str_append(str, "?");
    tk_snprintf(arg, sizeof(arg), "port=%d", flasher->tftp_settings.port);
    str_append(str, arg);
  } else {
    str_append(str, "serial(ymodem)://localhost/");
    str_append(str, flasher->filename.str);
    str_append(str, "?");
  }

  return RET_OK;
}

static void* transfer_thread(void* args) {
  str_t str;
  url_t *url = NULL;
  transferer_t *transferer = NULL;
  firmware_flasher_t* flasher = (firmware_flasher_t*)args;

  str_init(&str, 0);
  firmware_flasher_gen_url(flasher, &str);

  url = url_parse(str.str);
  url->dir = toLocaleString(url->dir);
  url->path = toLocaleString(url->path);
  url->filename = toLocaleString(url->filename);
  str_reset(&str);

  transferer = transferer_factory_create(url);
  if (transferer) {
    const char *msg = NULL;
    flasher->transferer = transferer;
    if (transferer_start(transferer, &msg)) {
    } else {
    }
    flasher->transferer = NULL;
    transferer_destroy(transferer);
  } else {
    log_debug("create transferer failed.");
  }
  url_unref(url);
  flasher->worker = NULL;

  return NULL;
}

ret_t firmware_flasher_init(void) {
  tftp_settings_t *tftp_settings = firmware_flasher_get_tftp_settings();
  serial_settings_t *serial_settings = firmware_flasher_get_serial_settings();

  memset(&s_flasher, 0x00, sizeof(s_flasher));

  serial_settings->parity = 0;
  serial_settings->byte_size = 8;
  serial_settings->stop_bits = 1;
  serial_settings->flow_control = 0;
  serial_settings->baud_rate = 115200;

  tftp_settings->port = 69;
  str_set(&(tftp_settings->ips), "127.0.0.1");
  
  transferer_tcp_register_creator_desc();
  transferer_serial_register_creator_desc();
  transferer_tftp_server_register_creator_desc();

  return RET_OK;
}

ret_t firmware_flasher_deinit(void) {
  str_reset(&(s_flasher.tftp_settings.ips));
  str_reset(&(s_flasher.serial_settings.port));

  memset(&s_flasher, 0x00, sizeof(s_flasher));

  return RET_OK;
}

bool_t firmware_flasher_is_ready_to_flash(void) {
  if (!file_exist(s_flasher.filename.str)) {
    return FALSE;
  }

  /*TODO*/

  return TRUE;
}

bool_t firmware_flasher_is_flashing(void) {
  return s_flasher.worker != NULL;
}

ret_t firmware_flasher_set_flash_type(flash_type_t type) {
  s_flasher.flash_type = type;

  return RET_OK;
}

ret_t firmware_flasher_set_file_name(value_t *v) {
  str_from_value(&(s_flasher.filename), v);

  return RET_OK;
}

int32_t firmware_flasher_get_progress(void) {
  if(s_flasher.transferer != NULL) {
    uint32_t total = transferer_get_total_size(s_flasher.transferer);
    uint32_t sent = transferer_get_sent_size(s_flasher.transferer);
    if(total > 0) {
      return (sent * 100)/total; 
    }
  }

  return 0;
}

tftp_settings_t *firmware_flasher_get_tftp_settings(void) {
  return &(s_flasher.tftp_settings);
}

serial_settings_t *firmware_flasher_get_serial_settings(void) {
  return &(s_flasher.serial_settings);
}

ret_t firmware_flasher_start_flash(void) { 
  return_value_if_fail(s_flasher.worker == NULL, RET_BAD_PARAMS);

  s_flasher.worker = tk_thread_create(transfer_thread, &s_flasher);
  tk_thread_start(s_flasher.worker);

  return RET_OK; 
}

ret_t firmware_flasher_cancel_flash(void) { 
  return_value_if_fail(s_flasher.worker != NULL, RET_BAD_PARAMS);

  return RET_OK; 
}

const char *firmware_flasher_get_file_name(void) {
  return s_flasher.filename.str;
}

flash_type_t firmware_flasher_get_flash_type(void) {
  return s_flasher.flash_type;
}
