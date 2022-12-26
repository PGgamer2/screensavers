cmake_minimum_required (VERSION 3.8)

project ("TEMPLATE_SCR")

find_package(SDL2 CONFIG REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable (${PROJECT_NAME} "init.cpp" "main.h" "screensaver.cpp" "settings.cpp"  "win_compat.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

if (WIN32)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
else()
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
endif()
endif()

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})