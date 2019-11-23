#include "tkc/mem.h"
#include "tftp_settings.h"
#include "common/firmware_flasher.h"
#include "mvvm/base/utils.h"
#include "tkc/utils.h"

/***************tftp_settings_view_model***************/
static ret_t tftp_settings_view_model_set_prop(object_t *obj, const char *name,
                                               const value_t *v) {
  tftp_settings_view_model_t *vm = (tftp_settings_view_model_t *)(obj);
  tftp_settings_t *tftp_settings = firmware_flasher_get_tftp_settings();

  if (tk_str_eq("ips", name)) {
    str_from_value(&(tftp_settings->ips), v);
  } else if (tk_str_eq("port", name)) {
    tftp_settings->port = value_int32(v);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static ret_t tftp_settings_view_model_get_prop(object_t *obj, const char *name,
                                               value_t *v) {
  tftp_settings_view_model_t *vm = (tftp_settings_view_model_t *)(obj);
  tftp_settings_t *tftp_settings = firmware_flasher_get_tftp_settings();

  if (tk_str_eq("ips", name)) {
    value_set_str(v, tftp_settings->ips.str);
  } else if (tk_str_eq("port", name)) {
    value_set_int32(v, tftp_settings->port);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static bool_t tftp_settings_view_model_can_exec(object_t *obj, const char *name,
                                                const char *args) {
  tftp_settings_view_model_t *vm = (tftp_settings_view_model_t *)(obj);

  if (tk_str_eq("apply", name)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static ret_t tftp_settings_view_model_exec(object_t *obj, const char *name,
                                           const char *args) {
  tftp_settings_view_model_t *vm = (tftp_settings_view_model_t *)(obj);

  if (tk_str_eq("apply", name)) {
    return RET_OK;
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }
}

static ret_t tftp_settings_view_model_on_destroy(object_t *obj) {
  tftp_settings_view_model_t *vm = (tftp_settings_view_model_t *)(obj);
  return_value_if_fail(vm != NULL, RET_BAD_PARAMS);

  return view_model_deinit(VIEW_MODEL(obj));
}

static const object_vtable_t s_tftp_settings_view_model_vtable = {
    .type = "tftp_settings",
    .desc = "tftp_settings",
    .size = sizeof(tftp_settings_view_model_t),
    .exec = tftp_settings_view_model_exec,
    .can_exec = tftp_settings_view_model_can_exec,
    .get_prop = tftp_settings_view_model_get_prop,
    .set_prop = tftp_settings_view_model_set_prop,
    .on_destroy = tftp_settings_view_model_on_destroy};

view_model_t *tftp_settings_view_model_create(navigator_request_t *req) {
  object_t *obj = object_create(&s_tftp_settings_view_model_vtable);

  return view_model_init(VIEW_MODEL(obj));
}
