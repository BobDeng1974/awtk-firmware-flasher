
#ifndef TK_FIRMWARE_FLASH_H
#define TK_FIRMWARE_FLASH_H

#include "mvvm/base/view_model.h"

BEGIN_C_DECLS

/**
 * @class firmware_flash_view_model_t
 *
 * view model of firmware_flash
 *
 */
typedef struct _firmware_flash_view_model_t {
  view_model_t view_model;
} firmware_flash_view_model_t;

/**
 * @method firmware_flash_view_model_create
 * 创建firmware_flash view model对象。
 *
 * @annotation ["constructor"]
 * @param {navigator_request_t*} req 请求参数。
 *
 * @return {view_model_t} 返回view_model_t对象。
 */
view_model_t *firmware_flash_view_model_create(navigator_request_t *req);

END_C_DECLS

#endif /*TK_FIRMWARE_FLASH_H*/
