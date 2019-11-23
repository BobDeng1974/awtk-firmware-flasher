#include "tkc/mem.h"
#include "flashing.h"
#include "mvvm/base/utils.h"
#include "tkc/utils.h"

/***************flashing***************/;

static inline flashing_t *flashing_create(void) {
  flashing_t *flashing = TKMEM_ZALLOC(flashing_t);
  return_value_if_fail(flashing != NULL, NULL);

  str_init(&(flashing->status), 10);

  return flashing;
}

static inline ret_t flashing_destroy(flashing_t *flashing) {
  return_value_if_fail(flashing != NULL, RET_BAD_PARAMS);

  str_reset(&(flashing->status));

  TKMEM_FREE(flashing);

  return RET_OK;
}

static bool_t flashing_can_exec_go(flashing_t *flashing, const char *args) {
  return TRUE;
}

static ret_t flashing_go(flashing_t *flashing, const char *args) {

  return RET_OBJECT_CHANGED;
}

static bool_t flashing_can_exec_cancel(flashing_t *flashing, const char *args) {
  return TRUE;
}

static ret_t flashing_cancel(flashing_t *flashing, const char *args) {

  return RET_OBJECT_CHANGED;
}

/***************flashing_view_model***************/

static ret_t flashing_view_model_set_prop(object_t *obj, const char *name,
                                          const value_t *v) {
  flashing_view_model_t *vm = (flashing_view_model_t *)(obj);
  flashing_t *flashing = vm->flashing;

  if (tk_str_eq("progress", name)) {
    flashing->progress = value_int32(v);
  } else if (tk_str_eq("status", name)) {
    str_from_value(&(flashing->status), v);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static ret_t flashing_view_model_get_prop(object_t *obj, const char *name,
                                          value_t *v) {
  flashing_view_model_t *vm = (flashing_view_model_t *)(obj);
  flashing_t *flashing = vm->flashing;

  if (tk_str_eq("progress", name)) {
    value_set_int32(v, flashing->progress);
  } else if (tk_str_eq("status", name)) {
    value_set_str(v, flashing->status.str);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static bool_t flashing_view_model_can_exec(object_t *obj, const char *name,
                                           const char *args) {
  flashing_view_model_t *vm = (flashing_view_model_t *)(obj);
  flashing_t *flashing = vm->flashing;

  if (tk_str_eq("go", name)) {
    return flashing_can_exec_go(flashing, args);
  } else if (tk_str_eq("cancel", name)) {
    return flashing_can_exec_cancel(flashing, args);
  } else {
    return FALSE;
  }
}

static ret_t flashing_view_model_exec(object_t *obj, const char *name,
                                      const char *args) {
  flashing_view_model_t *vm = (flashing_view_model_t *)(obj);
  flashing_t *flashing = vm->flashing;

  if (tk_str_eq("go", name)) {
    return flashing_go(flashing, args);
  } else if (tk_str_eq("cancel", name)) {
    return flashing_cancel(flashing, args);
  } else {
    log_debug("not found %s\n", name);
    return RET_NOT_FOUND;
  }
}

static ret_t flashing_view_model_on_destroy(object_t *obj) {
  flashing_view_model_t *vm = (flashing_view_model_t *)(obj);
  return_value_if_fail(vm != NULL, RET_BAD_PARAMS);

  flashing_destroy(vm->flashing);

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

view_model_t *flashing_view_model_create(navigator_request_t *req) {
  object_t *obj = object_create(&s_flashing_view_model_vtable);
  view_model_t *vm = view_model_init(VIEW_MODEL(obj));
  flashing_view_model_t *flashing_view_model = (flashing_view_model_t *)(vm);

  return_value_if_fail(vm != NULL, NULL);

  flashing_view_model->flashing = flashing_create();
  ENSURE(flashing_view_model->flashing != NULL);

  return vm;
}
