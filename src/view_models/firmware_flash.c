#include "tkc/mem.h"
#include "firmware_flash.h"
#include "common/firmware_flasher.h"
#include "common/types_def.h"
#include "mvvm/base/utils.h"
#include "tkc/utils.h"

/***************firmware_flash_view_model***************/
static ret_t firmware_flash_view_model_set_prop(object_t *obj, const char *name,
                                                const value_t *v) {
  firmware_flash_view_model_t *vm = (firmware_flash_view_model_t *)(obj);

  if (tk_str_eq("flash_type", name)) {
    firmware_flasher_set_flash_type(value_int32(v));
  } else if (tk_str_eq("filename", name)) {
    firmware_flasher_set_file_name(v);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static ret_t firmware_flash_view_model_get_prop(object_t *obj, const char *name,
                                                value_t *v) {
  firmware_flash_view_model_t *vm = (firmware_flash_view_model_t *)(obj);

  if (tk_str_eq("flash_type", name)) {
    value_set_int32(v, firmware_flasher_get_flash_type());
  } else if (tk_str_eq("filename", name)) {
    value_set_str(v, firmware_flasher_get_file_name());
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static bool_t firmware_flash_view_model_can_exec(object_t *obj,
                                                 const char *name,
                                                 const char *args) {
  firmware_flash_view_model_t *vm = (firmware_flash_view_model_t *)(obj);

  if (tk_str_eq("choose", name)) {
    return TRUE;
  } else if (tk_str_eq("settings", name)) {
    return TRUE;
  } else if (tk_str_eq("go", name)) {
    return firmware_flasher_is_ready_to_flash();
  } else {
    return FALSE;
  }
}

static ret_t firmware_flash_view_model_exec(object_t *obj, const char *name,
                                            const char *args) {
  firmware_flash_view_model_t *vm = (firmware_flash_view_model_t *)(obj);

  if (tk_str_eq("choose", name)) {
    str_t str;
    str_init(&str, 0);
    if (navigator_pick_file("Pick firmware", "*/*", FALSE, &str) == RET_OK) {
      value_t v;
      value_set_str(&v, str.str);
      firmware_flasher_set_file_name(&v);
    }
    str_reset(&str);

    return RET_OBJECT_CHANGED;
  } else if (tk_str_eq("settings", name)) {
    if (firmware_flasher_get_flash_type() == FLASH_TYPE_TFTP) {
      navigator_to("tftp_settings");
    } else {
      navigator_to("serial_settings");
    }
  } else if (tk_str_eq("go", name)) {
    return navigator_to("flashing");
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }
}

static ret_t firmware_flash_view_model_on_destroy(object_t *obj) {
  firmware_flash_view_model_t *vm = (firmware_flash_view_model_t *)(obj);
  return_value_if_fail(vm != NULL, RET_BAD_PARAMS);

  return view_model_deinit(VIEW_MODEL(obj));
}

static const object_vtable_t s_firmware_flash_view_model_vtable = {
    .type = "firmware_flash",
    .desc = "firmware_flash",
    .size = sizeof(firmware_flash_view_model_t),
    .exec = firmware_flash_view_model_exec,
    .can_exec = firmware_flash_view_model_can_exec,
    .get_prop = firmware_flash_view_model_get_prop,
    .set_prop = firmware_flash_view_model_set_prop,
    .on_destroy = firmware_flash_view_model_on_destroy};

view_model_t *firmware_flash_view_model_create(navigator_request_t *req) {
  object_t *obj = object_create(&s_firmware_flash_view_model_vtable);

  return view_model_init(VIEW_MODEL(obj));
}
