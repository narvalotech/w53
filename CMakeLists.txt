# Find Zephyr. This also loads Zephyr's build system.
cmake_minimum_required(VERSION 3.13.1)

list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(BOARD w53_cpuapp)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})

project(rgbwatch)

set(CMAKE_BUILD_TYPE Release)

set(APP_SRC
  src/main.c
  src/disp.c
  src/clock.c
  src/accel.c
  src/screen.c
  src/pinmux.c
  src/board.c
  src/state.c
  src/stopwatch.c
  src/countdown.c
  src/motor.c
  src/ble.c
  src/cts.c
  src/calendar.c
  src/alarm.c
  )

set(APP_INCLUDE
  include
  )

target_sources(app PRIVATE ${APP_SRC})
target_include_directories(app PRIVATE ${APP_INCLUDE})
