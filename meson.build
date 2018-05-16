project(
  'LittlePong',
  ['cpp'],
  default_options : ['cpp_std=c++1z', 'buildtype=debugoptimized']
)

src = [
  'src/main.cc',
  'src/game.cc',
]

littleton_sp = subproject('Littleton', default_options : ['buildtype=release'])
littleton_dep = littleton_sp.get_variable('littleton_dep')

cc = meson.get_compiler('cpp')

executable(
  'littlepong',
  src,
  dependencies: [littleton_dep],
  install: true,
)


