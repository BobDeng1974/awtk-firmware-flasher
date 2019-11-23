import os
import sys
import platform

sys.path.insert(0, '../awtk/')
import awtk_config as awtk

APP_ROOT    = os.path.normpath(os.getcwd())
APP_BIN_DIR = os.path.join(APP_ROOT, 'bin')
APP_LIB_DIR = os.path.join(APP_ROOT, 'lib')
APP_SRC_DIR = os.path.join(APP_ROOT, 'src')

os.environ['APP_ROOT'] = APP_ROOT;
os.environ['BIN_DIR'] = APP_BIN_DIR;
os.environ['LIB_DIR'] = APP_LIB_DIR;
os.environ['APP_SRC'] = APP_SRC_DIR;

APP_CCFLAGS = '-DWIN32_LEAN_AND_MEAN '
APP_LIBS = ['assets']
APP_LIBPATH = [APP_LIB_DIR]
APP_CPPPATH = [APP_SRC_DIR, os.path.join(APP_SRC_DIR, 'common'), 
  os.path.join(APP_SRC_DIR, 'view_models'),
  os.path.join(APP_SRC_DIR, "flashers"),
  os.path.join(APP_SRC_DIR, "flashers/tftp"),
  os.path.join(APP_SRC_DIR, "flashers/tools"),
  os.path.join(APP_SRC_DIR, "flashers/common"),
  os.path.join(APP_SRC_DIR, "flashers/streams"),
  os.path.join(APP_SRC_DIR, "flashers/ymodem"),
  os.path.join(APP_SRC_DIR, "flashers/transferers"),
  os.path.join(APP_SRC_DIR, "flashers/serial/src"),
  os.path.join(APP_SRC_DIR, "flashers/serial/include"),
  os.path.join(APP_SRC_DIR, "flashers/serial/src/impl"),
  os.path.join(APP_SRC_DIR, "flashers/serial/src/impl/list_ports"),
]

MVVM_ROOT = os.path.normpath(os.path.join(APP_ROOT, '../awtk-mvvm'))
MVVM_LIB_DIR = os.path.join(MVVM_ROOT, 'lib')
MVVM_SRC_DIR = os.path.join(MVVM_ROOT, 'src')

MVVM_LIBS = ['mvvm', 'jerryscript', 'nfd']
MVVM_CPPPATH = [MVVM_SRC_DIR]
MVVM_LIBPATH = [MVVM_LIB_DIR]

DefaultEnvironment(
  LINKFLAGS = awtk.LINKFLAGS,
  CPPPATH   = awtk.CPPPATH + MVVM_CPPPATH + APP_CPPPATH,
  LIBS      = APP_LIBS + MVVM_LIBS + awtk.LIBS,
  LIBPATH   = APP_LIBPATH + MVVM_LIBPATH + awtk.LIBPATH,
  CCFLAGS   = APP_CCFLAGS + awtk.CCFLAGS, 
  OS_SUBSYSTEM_CONSOLE=awtk.OS_SUBSYSTEM_CONSOLE,
  OS_SUBSYSTEM_WINDOWS=awtk.OS_SUBSYSTEM_WINDOWS)

SConscript(['src/SConscript', 'tests/SConscript'])

