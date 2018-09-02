cmake_minimum_required(VERSION 3.5.2)

project(fluttercast)

set(FLUTTER_ENGINE_SHA 949fb0fb4cbe4bf4753d68264b9d9cc966ba7058)

set(FLUTTER_EMBEDDER_ARTIFACTS_ZIP ${CMAKE_BINARY_DIR}/flutter_embedder_${FLUTTER_ENGINE_SHA}.zip)
set(FLUTTER_ARTIFACTS_ZIP          ${CMAKE_BINARY_DIR}/flutter_artifact_${FLUTTER_ENGINE_SHA}.zip)
set(FLUTTER_BUCKET_BASE "https://storage.googleapis.com/flutter_infra/flutter")

# Download and setup the Flutter Engine.
if(NOT EXISTS ${FLUTTER_EMBEDDER_ARTIFACTS_ZIP})
  file(DOWNLOAD
    ${FLUTTER_BUCKET_BASE}/${FLUTTER_ENGINE_SHA}/linux-x64/linux-x64-embedder
    ${FLUTTER_EMBEDDER_ARTIFACTS_ZIP}
    SHOW_PROGRESS
  )
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf ${FLUTTER_EMBEDDER_ARTIFACTS_ZIP}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()

if(NOT EXISTS ${FLUTTER_ARTIFACTS_ZIP})
  file(DOWNLOAD
    ${FLUTTER_BUCKET_BASE}/${FLUTTER_ENGINE_SHA}/linux-x64/artifacts.zip
    ${FLUTTER_ARTIFACTS_ZIP}
    SHOW_PROGRESS
  )
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf ${FLUTTER_ARTIFACTS_ZIP}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()

# Find package dependencies.
find_package(PkgConfig)
pkg_check_modules(VNC_SERVER REQUIRED libvncserver) # libvncserver-dev

# Executable
file(GLOB_RECURSE FLUTTER_CAST_SRC
  "src/*.cc"
  "src/*.h"
)

link_directories(${CMAKE_BINARY_DIR})

add_executable(fluttercast ${FLUTTER_CAST_SRC})

target_link_libraries(fluttercast
  ${VNC_SERVER_LIBRARIES}
  flutter_engine
)

target_include_directories(fluttercast
  PRIVATE
  ${VNC_SERVER_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}
)
