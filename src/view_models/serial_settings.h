
#ifndef TK_SERIAL_SETTINGS_H
#define TK_SERIAL_SETTINGS_H

#include "common/types_def.h"
#include "mvvm/base/view_model.h"

BEGIN_C_DECLS

/**
 * @class serial_settings_view_model_t
 *
 * view model of serial_settings
 *
 */
typedef struct _serial_settings_view_model_t {
  view_model_t view_model;

} serial_settings_view_model_t;

/**
 * @method serial_settings_view_model_create
 * 创建serial_settings view model对象。
 *
 * @annotation ["constructor"]
 * @param {navigator_request_t*} req 请求参数。
 *
 * @return {view_model_t} 返回view_model_t对象。
 */
view_model_t *serial_settings_view_model_create(navigator_request_t *req);

END_C_DECLS

#endif /*TK_SERIAL_SETTINGS_H*/
