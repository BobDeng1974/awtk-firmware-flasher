#include "flashing.h"
#include "mvvm/base/utils.h"
#include "mvvm/base/navigator.h"
#include "base/timer.h"
#include "tkc/utils.h"
#include "tkc/mem.h"
#include "common/firmware_flasher.h"

/***************flashing_view_model***************/

static ret_t flashing_view_model_set_prop(object_t *obj, const char *name,
                                          const value_t *v) {
  return RET_OK;
}

static ret_t flashing_view_model_get_prop(object_t *obj, const char *name,
                                          value_t *v) {
  flashing_view_model_t *vm = (flashing_view_model_t *)(obj);

  if (tk_str_eq("progress", name)) {
    value_set_int32(v, firmware_flasher_get_progress());
  } else if (tk_str_eq("status", name)) {
    value_set_str(v, vm->status.str);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static bool_t flashing_view_model_can_exec(object_t *obj, const char *name,
                                           const char *args) {
  if (tk_str_eq("go", name)) {
    return !firmware_flasher_is_flashing();
  } else if (tk_str_eq("cancel", name)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static ret_t flashing_view_model_exec(object_t *obj, const char *name,
                                      const char *args) {
  if (tk_str_eq("go", name)) {
    firmware_flasher_start_flash();
    return RET_OBJECT_CHANGED;
  } else if (tk_str_eq("cancel", name)) {
    if(firmware_flasher_is_flashing()) {
      firmware_flasher_cancel_flash();
    } else {
      navigator_back();
    }
    return RET_OBJECT_CHANGED;
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }
}

static ret_t flashing_view_model_on_destroy(object_t *obj) {
  flashing_view_model_t *vm = (flashing_view_model_t *)(obj);
  return_value_if_fail(vm != NULL, RET_BAD_PARAMS);

  str_reset(&(vm->status));
  timer_remove(vm->timer_id);

  return view_model_deinit(VIEW_MODEL(obj));
}

static const object_vtable_t s_flashing_view_model_vtable = {
    .type = "flashing",
    .desc = "flashing",
    .size = sizeof(flashing_view_model_t),
    .exec = flashing_view_model_exec,
    .can_exec = flashing_view_model_can_exec,
    .get_prop = flashing_view_model_get_prop,
    .set_prop = flashing_view_model_set_prop,
    .on_destroy = flashing_view_model_on_destroy};

static ret_t update_status_in_timer(const timer_info_t* info) {
  firmware_flasher_t* flasher = (firmware_flasher_t*)(info->ctx);

  object_notify_changed(OBJECT(flasher));

  return RET_REPEAT;
}

view_model_t *flashing_view_model_create(navigator_request_t *req) {
  char basename[MAX_PATH+1];
  object_t *obj = object_create(&s_flashing_view_model_vtable);
  view_model_t *vm = view_model_init(VIEW_MODEL(obj));
  flashing_view_model_t *flashing_view_model = (flashing_view_model_t *)(vm);

  return_value_if_fail(vm != NULL, NULL);

  str_init(&(flashing_view_model->status), MAX_PATH + 1);
  memset(basename, 0x00, sizeof(basename));
  path_basename(firmware_flasher_get_file_name(), basename, MAX_PATH);
  str_set(&(flashing_view_model->status), basename);
  flashing_view_model->timer_id = timer_add(update_status_in_timer, flashing_view_model, 500);

  return vm;
}
