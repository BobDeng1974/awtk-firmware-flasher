/**
 * File:   firmware_flasher.h
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

#ifndef TK_FIRMWARE_FLASHER_H
#define TK_FIRMWARE_FLASHER_H

#include "tkc/thread.h"
#include "transferer_factory.h"
#include "common/types_def.h"

BEGIN_C_DECLS

/**
 * @class firmware_flasher_t
 *
 * 一些全局单例对象。
 *
 */
typedef struct _firmware_flasher_t {
  str_t filename;
  flash_type_t flash_type;
  tftp_settings_t tftp_settings;
  serial_settings_t serial_settings;

  tk_thread_t* worker;
  transferer_t *transferer;
} firmware_flasher_t;

/**
 * @method firmware_flasher_init
 * 初始化。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t firmware_flasher_init(void);

ret_t firmware_flasher_start_flash(void);
ret_t firmware_flasher_cancel_flash(void);
bool_t firmware_flasher_is_ready_to_flash(void);
bool_t firmware_flasher_is_flashing(void);

ret_t firmware_flasher_set_flash_type(flash_type_t type);
ret_t firmware_flasher_set_file_name(value_t *v);

int32_t firmware_flasher_get_progress(void);
tftp_settings_t *firmware_flasher_get_tftp_settings(void);
serial_settings_t *firmware_flasher_get_serial_settings(void);
const char *firmware_flasher_get_file_name(void);
flash_type_t firmware_flasher_get_flash_type(void);

/**
 * @method firmware_flasher_deinit
 * ~初始化。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t firmware_flasher_deinit(void);

END_C_DECLS

#endif /*TK_FIRMWARE_FLASHER_H*/
