#ifndef MODELS_TYPES_DEF_H
#define MODELS_TYPES_DEF_H

#include "tkc/types_def.h"

#define trace printf

#ifndef errno_t
typedef int errno_t;
#endif

#define ERRNO_OK 0
#define ERRNO_FAIL -1
#define ERRNO_NO_SPACE -2
#define ERRNO_NOT_FOUND -3
#define ERRNO_NO_PERMISSION -4
#define ERRNO_INVALID_PARAMS -5

#ifndef WIN32
#define strncpy_s(a, al, b, bl) strncpy(a, b, bl);
#endif // WIN32

typedef void (*GObjectDestroyFunc)(void *obj);

typedef struct _GObject {
  GObjectDestroyFunc destroy;
  void *obj;
} GObject;

static inline GObject gobject_init(GObjectDestroyFunc destroy, void *obj) {
  GObject gobject;
  gobject.destroy = destroy;
  gobject.obj = obj;

  return gobject;
}

typedef int (*hash_func_t)(const void *data, size_t len);
typedef int (*compare_func_t)(const void *a, const void *b, size_t len);
typedef bool_t (*visit_func_t)(void *ctx, const void *data);

#define ELM_NR(a) (sizeof(a) / sizeof(a[0]))

void socketInit();
void socketDeinit();
void msleep(int ms);
void closeSocket(int sock);

#endif // MODELS_TYPES_DEF_H
