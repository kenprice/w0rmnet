cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(w0rmnet)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG REQUIRED glib-2.0)
if (GLIB_PKG_FOUND)
    message(Found glib-2.0)
    include_directories(${GLIB_PKG_INCLUDE_DIRS})
endif()

# Our Project

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/helper/"
add_executable(${PROJECT_NAME} app.c
        graphics/sprites.h
        graphics/sprites.c
        entities/entities.h
        components/component_registry.h
        components/component_registry.c
        utils/uuid.h
        systems/device_rendering_system.h
        systems/device_rendering_system.c
        components/device.h
        components/device.c
        utils/uuid.c
        components/utils/device_id.h
        components/utils/device_id_wordlist.h
        components/utils/device_id.c
        utils/random.h
        utils/random.c
        entities/machine.h
        entities/machine.c
        entities/router.h
        entities/router.c
        components/connection.c
        components/connection.h
        systems/packet_routing_system.h
        systems/packet_routing_system.c
        components/packet.h
        components/packet.c
        systems/utils/rendering.h
        systems/utils/rendering.c
        utils/timer.h
        utils/timer.c
        components/sprite.h
        components/sprite.c
        components/route_table.h
        components/route_table.c
        components/process.h
        components/process.c
        systems/process_running_system.h
        systems/process_running_system.c
        systems/device_ui_system.h
        systems/device_ui_system.c
        world/world_map.h
        world/world_map.c)
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} ${GLIB_PKG_LINK_LIBRARIES})

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()