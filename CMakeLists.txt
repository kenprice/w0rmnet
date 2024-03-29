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
        components/component_registry.h
        components/component_registry.c
        utils/uuid.h
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
        systems/packet_routing_system.h
        systems/packet_routing_system.c
        components/packet.h
        components/packet.c
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
        systems/main_gui_system.h
        systems/main_gui_system.c
        world/world_map.h
        world/world_map.c
        graphics/tiles.h
        graphics/tiles.c
        systems/process_runner/ping_runner.h
        systems/process_runner/ping_runner.c
        systems/process_runner/proc_runner_table.h
        systems/process_runner/proc_runner_table.c
        systems/process_runner/netscan_runner.h
        systems/process_runner/netscan_runner.c
        lib/style_cyber.h
        world/world_state.h
        world/world_state.c
        systems/process_runner/login_runner.h
        systems/process_runner/login_runner.c
        systems/main_gui_system/area_viewer_window.h
        systems/main_gui_system/area_viewer_window.c
        systems/ui/ui_window.h
        systems/ui/ui_window.c
        systems/ui/device_info_panel.h
        systems/ui/device_info_panel.c
        systems/main_gui_system/isometric_map_rendering.h
        systems/main_gui_system/isometric_map_rendering.c
        components/wire.h
        components/wire.c
        components/polygon.h
        components/polygon.c
        lib/log/log.h
        lib/log/log.c
        components/logger.h
        components/logger.c
        systems/ui/custom_raygui.h
        systems/ui/custom_raygui.c
        world/world_gen.h
        world/world_gen.c
        systems/botnet_system.h
        systems/botnet_system.c
        systems/main_gui_system/toolwindows/network_map_toolwindow.h
        systems/main_gui_system/toolwindows/network_map_toolwindow.c
        systems/main_gui_system/toolwindow.h
        systems/main_gui_system/toolwindow.c
        systems/main_gui_system/minimap.h
        systems/main_gui_system/minimap.c
        lib/raymath.h
        entities/network_switch.c
        entities/network_switch.h
        systems/process_runner/check_origin_runner.c
        systems/process_runner/check_origin_runner.h
        components/position.c
        components/position.h
        events/device_events.c
        events/device_events.h
        systems/main_gui_system/recent_events.h
        systems/main_gui_system/recent_events.c
        systems/main_gui_system/toolwindows/worms_toolwindow.c
        systems/main_gui_system/toolwindows/worms_toolwindow.h
        components/file_storage.h
        store/files.h
        store/exploits.h
        store/programs.h
        store/programs.c
        store/exploits.c
        components/loot.h
        store/cred_dumps.c
        store/cred_dumps.h
        events/player_events.c
        events/player_events.h
        systems/world_state_system.c
        systems/world_state_system.h
        events/events.h
        events/events.c
        utils/math_macros.h
        lib/text_rectangle_bounds.c
        lib/text_rectangle_bounds.h
        systems/main_gui_system/worms_window.c
        systems/main_gui_system/worms_window.h
        store/worms.h
        systems/worm_system.c
        systems/worm_system.h
        store/worms.c
        components/infection.h
        components/infection.c
        events/worm_events.c
        events/worm_events.h
        entities/helpers.c
        entities/helpers.h
        components/known_hosts.c
        components/known_hosts.h
        systems/ui/ui_consts.h
        graphics/shaders.h
        graphics/shaders.c
)
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