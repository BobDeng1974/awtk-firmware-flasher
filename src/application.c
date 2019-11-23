/**
 * File:   application.c
 * Author: AWTK Develop Team
 * Brief:  application
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

#include "awtk.h"
#include "mvvm/mvvm.h"

#include "view_models/firmware_flash.h"
#include "view_models/flashing.h"
#include "view_models/serial_settings.h"
#include "view_models/tftp_settings.h"

#include "common/firmware_flasher.h"

ret_t application_init() {
  firmware_flasher_init();

  view_model_factory_register("flashing", flashing_view_model_create);
  view_model_factory_register("firmware_flash",
                              firmware_flash_view_model_create);
  view_model_factory_register("serial_settings",
                              serial_settings_view_model_create);
  view_model_factory_register("tftp_settings", tftp_settings_view_model_create);

  return navigator_to("firmware_flash");
}

#include "awtk_main.c"
