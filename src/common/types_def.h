/**
 * File:   types_def.h
 * Author: AWTK Develop Team
 * Brief:  types_def
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

#ifndef TK_FLASH_TYPES_DEF_H
#define TK_FLASH_TYPES_DEF_H

#include "tkc/str.h"

BEGIN_C_DECLS

typedef enum _flash_type_t {
  FLASH_TYPE_TFTP = 0,
  FLASH_TYPE_SERIAL_Y_MODEM
} flash_type_t;

/**
 * @class serial_settings_t
 *
 * serial_settings
 *
 */
typedef struct _serial_settings_t {
  str_t port;
  int32_t baud_rate;
  int32_t byte_size;
  int32_t stop_bits;
  int32_t flow_control;
  int32_t parity;
} serial_settings_t;

/**
 * @class tftp_settings_t
 *
 * tftp_settings
 *
 */
typedef struct _tftp_settings_t {
  str_t ips;
  int32_t port;
} tftp_settings_t;

END_C_DECLS

#endif /*TK_FLASH_TYPES_DEF_H*/
