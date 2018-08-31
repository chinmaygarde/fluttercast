cmake_minimum_required(VERSION 3.5.2)

project(fluttercast)

find_package(PkgConfig)
pkg_check_modules(VNC_SERVER REQUIRED libvncserver)

# Executable

file(GLOB_RECURSE FLUTTER_CAST_SRC
  "src/*.cc"
  "src/*.h"
)

add_executable(fluttercast ${FLUTTER_CAST_SRC})
target_link_libraries(fluttercast ${VNC_SERVER_LIBRARIES})
target_include_directories(fluttercast PRIVATE ${VNC_SERVER_INCLUDE_DIRS})

