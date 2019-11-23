
#ifndef TK_TFTP_SETTINGS_H
#define TK_TFTP_SETTINGS_H

#include "common/types_def.h"
#include "mvvm/base/view_model.h"

BEGIN_C_DECLS

/**
 * @class tftp_settings_view_model_t
 *
 * view model of tftp_settings
 *
 */
typedef struct _tftp_settings_view_model_t {
  view_model_t view_model;

} tftp_settings_view_model_t;

/**
 * @method tftp_settings_view_model_create
 * 创建tftp_settings view model对象。
 *
 * @annotation ["constructor"]
 * @param {navigator_request_t*} req 请求参数。
 *
 * @return {view_model_t} 返回view_model_t对象。
 */
view_model_t *tftp_settings_view_model_create(navigator_request_t *req);

END_C_DECLS

#endif /*TK_TFTP_SETTINGS_H*/
