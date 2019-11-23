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

#include "firmware_flasher.h"

static firmware_flasher_t s_flasher;

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
  ;

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

ret_t firmware_flasher_set_flash_type(flash_type_t type) {
  s_flasher.flash_type = type;

  return RET_OK;
}

ret_t firmware_flasher_set_file_name(value_t *v) {
  str_from_value(&(s_flasher.filename), v);

  return RET_OK;
}

int32_t firmware_flasher_get_progress(void) {
  /*TODO*/
  return 0;
}

tftp_settings_t *firmware_flasher_get_tftp_settings(void) {
  return &(s_flasher.tftp_settings);
}

serial_settings_t *firmware_flasher_get_serial_settings(void) {
  return &(s_flasher.serial_settings);
}

ret_t firmware_flasher_start_flash(void) { return RET_OK; }

ret_t firmware_flasher_cancel_flash(void) { return RET_OK; }

const char *firmware_flasher_get_file_name(void) {
  return s_flasher.filename.str;
}

flash_type_t firmware_flasher_get_flash_type(void) {
  return s_flasher.flash_type;
}
