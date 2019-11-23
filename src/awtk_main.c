/**
 * File:   demo_main.c
 * Author: AWTK Develop Team
 * Brief:  demo main
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-02-16 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "awtk.h"
#include "mvvm/mvvm.h"

BEGIN_C_DECLS
ret_t assets_init(void);
END_C_DECLS

#if defined(WIN32)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance,
                   LPSTR lpcmdline, int ncmdshow) {
#else
int main(void) {
#endif
  char app_root[MAX_PATH + 1];

  TK_ENABLE_CONSOLE();
  path_app_root(app_root);
  tk_init(640, 480, APP_DESKTOP, NULL, app_root);
  system_info_set_default_font(system_info(), "default_full");

  tk_ext_widgets_init();

  assets_init();
  mvvm_init();
  window_open("system_bar");
  application_init();

  tk_run();

  mvvm_deinit();

  return 0;
}
