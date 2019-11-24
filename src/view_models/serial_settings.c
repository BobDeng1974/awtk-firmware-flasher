#include "tkc/mem.h"
#include "serial_settings.h"
#include "common/list_ports.h"
#include "common/firmware_flasher.h"
#include "mvvm/base/utils.h"
#include "tkc/utils.h"

/***************serial_settings_view_model***************/

static ret_t serial_settings_view_model_set_prop(object_t *obj,
                                                 const char *name,
                                                 const value_t *v) {
  serial_settings_view_model_t *vm = (serial_settings_view_model_t *)(obj);
  serial_settings_t *serial_settings = firmware_flasher_get_serial_settings();

  if (tk_str_eq("port", name)) {
    str_from_value(&(serial_settings->port), v);
  } else if (tk_str_eq("baud_rate", name)) {
    serial_settings->baud_rate = value_int32(v);
  } else if (tk_str_eq("byte_size", name)) {
    serial_settings->byte_size = value_int32(v);
  } else if (tk_str_eq("stop_bits", name)) {
    serial_settings->stop_bits = value_int32(v);
  } else if (tk_str_eq("flow_control", name)) {
    serial_settings->flow_control = value_int32(v);
  } else if (tk_str_eq("parity", name)) {
    serial_settings->parity = value_int32(v);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static ret_t serial_settings_view_model_get_prop(object_t *obj,
                                                 const char *name, value_t *v) {
  serial_settings_view_model_t *vm = (serial_settings_view_model_t *)(obj);
  serial_settings_t *serial_settings = firmware_flasher_get_serial_settings();

  if (tk_str_eq("port", name)) {
    value_set_str(v, serial_settings->port.str);
  } else if (tk_str_eq("ports", name)) {
    str_t ports;
    str_init(&ports, 200);

    serial_ports_list(&ports);
    value_dup_str(v, ports.str);
    str_reset(&ports);

  } else if (tk_str_eq("baud_rate", name)) {
    value_set_int32(v, serial_settings->baud_rate);
  } else if (tk_str_eq("byte_size", name)) {
    value_set_int32(v, serial_settings->byte_size);
  } else if (tk_str_eq("stop_bits", name)) {
    value_set_int32(v, serial_settings->stop_bits);
  } else if (tk_str_eq("flow_control", name)) {
    value_set_int32(v, serial_settings->flow_control);
  } else if (tk_str_eq("parity", name)) {
    value_set_int32(v, serial_settings->parity);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static bool_t serial_settings_view_model_can_exec(object_t *obj,
                                                  const char *name,
                                                  const char *args) {
  serial_settings_view_model_t *vm = (serial_settings_view_model_t *)(obj);

  if (tk_str_eq("apply", name)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static ret_t serial_settings_view_model_exec(object_t *obj, const char *name,
                                             const char *args) {
  serial_settings_view_model_t *vm = (serial_settings_view_model_t *)(obj);

  if (tk_str_eq("apply", name)) {
    return RET_OK;
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }
}

static ret_t serial_settings_view_model_on_destroy(object_t *obj) {
  serial_settings_view_model_t *vm = (serial_settings_view_model_t *)(obj);
  return_value_if_fail(vm != NULL, RET_BAD_PARAMS);

  return view_model_deinit(VIEW_MODEL(obj));
}

static const object_vtable_t s_serial_settings_view_model_vtable = {
    .type = "serial_settings",
    .desc = "serial_settings",
    .size = sizeof(serial_settings_view_model_t),
    .exec = serial_settings_view_model_exec,
    .can_exec = serial_settings_view_model_can_exec,
    .get_prop = serial_settings_view_model_get_prop,
    .set_prop = serial_settings_view_model_set_prop,
    .on_destroy = serial_settings_view_model_on_destroy};

view_model_t *serial_settings_view_model_create(navigator_request_t *req) {
  object_t *obj = object_create(&s_serial_settings_view_model_vtable);

  return view_model_init(VIEW_MODEL(obj));
}
