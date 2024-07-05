project('Ytrizja Multimedia file name normalization tool', 'c',
  default_options : ['c_std=c17'],
  version : '0.4.0',
  license : 'Apache-2.0',
  meson_version : '>=0.50')

criterion_dep = dependency('criterion', main: true, required: false)

compiler = meson.get_compiler('c')
res_sys_dirent = compiler.has_header('sys/dirent.h')

cflags = [
  '-D_DEFAULT_SOURCE',
  '-D__BSD_VISIBLE=1',
  '-D_GNU_SOURCE',
  '-D_POSIX_C_SOURCE=200809L',
  '-D_FILE_OFFSET_BITS=64',
  '-fno-plt',
  '-fno-unwind-tables',
  '-ggdb3',
  '-Wall',
  '-Werror=return-type',
  '-Werror=type-limits',
  '-Werror=incompatible-pointer-types',
  '-Werror=implicit-function-declaration'
]

if res_sys_dirent
  cflags += ['-DYHAS_SYS_DIRENT']
endif

incdot = include_directories('.')

lutils = static_library('mmzx-utils',
  'utils.c',
  c_args : cflags,
  include_directories : incdot,
  install: false)

if criterion_dep.found()
  test('tests', executable('mmzx-tests', 'tests.c',
    dependencies: [criterion_dep],
    link_with: [lutils],
    c_args : cflags,
    include_directories : incdot,
    install: false))
endif

executable('mmzx',
  'mmzx.c',
  link_with: [lutils],
  c_args : cflags,
  include_directories : incdot,
  install: true)
