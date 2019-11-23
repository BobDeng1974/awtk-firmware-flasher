﻿
#ifndef TK_FLASHING_H
#define TK_FLASHING_H

#include "mvvm/base/view_model.h"

BEGIN_C_DECLS

/**
 * @class flashing_t
 *
 * flashing
 *
 */
typedef struct _flashing_t {
  int32_t progress;
  str_t status;
} flashing_t;

/**
 * @class flashing_view_model_t
 *
 * view model of flashing
 *
 */
typedef struct _flashing_view_model_t {
  view_model_t view_model;

  /*model object*/
  flashing_t *flashing;
} flashing_view_model_t;

/**
 * @method flashing_view_model_create
 * 创建flashing view model对象。
 *
 * @annotation ["constructor"]
 * @param {navigator_request_t*} req 请求参数。
 *
 * @return {view_model_t} 返回view_model_t对象。
 */
view_model_t *flashing_view_model_create(navigator_request_t *req);

END_C_DECLS

#endif /*TK_FLASHING_H*/
