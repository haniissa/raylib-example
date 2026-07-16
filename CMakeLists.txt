cmake_minimum_required(VERSION 3.15)
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)
set(APPEND PROJECT_NAME ${PROJECT_NAME})

include(FetchContent)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(_VULKAN_MSYS2_PREFIX "C:/msys64/mingw64" CACHE PATH "msys2 MinGW64 prefix for Vulkan")

list(APPEND CMAKE_PREFIX_PATH "C:/msys64/mingw64")

#Find RayLib
find_package(raylib CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(vk-bootstrap QUIET)
find_package(Vulkan REQUIRED)
# find_package(googletest CONFIG QUIET)

# if(NOT GTest_FOUND)
#   message(STATUS "❌----googletest-----❌ not found")
#   message(STATUS "well be instll it now.😀")
#   find_library()
#   # FetchContent_Declare(
#   #   googletest
#   #   GIT_REPOSITORY https://github.com/google/googletest.git
#   #   GIT_TAG v1.14.0
#   #   FIND_PACKAGE_ARGS NAMES GTest # Try find package first
#   # )
#   # FetChContent_MakeAvailable(googletest)
# else()
#   message(STATUS "googletest FOUND ✅")
# endif()
# #===========================================================================
if(fmt_FOUND)
  message(STATUS "Found fmt via Cmake CONFIG")
  set(FMT_LIBRARIES fmt)
else()
  message(STATUS "fmt not found on system , we well instal it")
  message(STATUS "we well install fmt library now!❣️")
  find_library(FMT_LIBRARIES NAMES fmt PATHS "C:/msys64/mingw64/lib")
  find_path(FMT_LIBRARIES NAMES fmt.h  PATHS "C:/msys64/mingw64/include")
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG    11.0.0
    FIND_PACKAGE_ARGS
  )
  FetchContent_MakeAvailable(fmt)
endif()
#first check system/installed packages
#if not found on system , install it

if(raylib_FOUND)
  message(STATUS "Found raylib via CMake CONFIG")
  set(RAYLIB_LIBRARIES raylib)
else()
  # Fallback to manual discovery
  find_library(RAYLIB_LIBRARIES NAMES raylib PATHS "C:/msys64/mingw64/lib")
  find_path(RAYLIB_INCLUDE_DIRS NAMES raylib.h PATHS "C:/msys64/mingw64/include")

  if(RAYLIB_LIBRARIES AND RAYLIB_INCLUDE_DIRS)
    message(STATUS "Found raylib manually: ${RAYLIB_LIBRARIES}")
  else()
    message(FATAL_ERROR "Could Not find raylib library or headrs!")
  endif()
endif()
if(vk-bootstrap_FOUND)
  message(STATUS "Found vk-bootstrap via Cmake CONFIG 🎉")
  set(VK_BOOSTRAB_LIBRARIES  vk-bootstrap )
else()
  message(STATUS "Could not find VkBootstrap library")
  find_library(VK_BOOSTRAB_LIBRARIES NAMES vk-bootstrap PATH "C:/msys64/mingw64/lib")
  find_path(VK_BOOSTRAB_LIBRARIES NAMES vk-bootstrap PATHS "C:/msys64/ming64/include")

  FetchContent_Declare(VkBoostrap
    GIT_REPOSITORY https://github.com/charles-lunarg/VkBoostrap
    GIT_TAG v1.3.290
  )
  FetchContent_MakeAvailable(vk-bootstrap)

endif()

#Executable
add_executable(${PROJECT_NAME} src/main.cpp
  src/Enemy.cpp
  src/Game.cpp
  src/Player.cpp
  src/Bullet.cpp
  src/Vulkancompute.cpp
)

if(RAYLIB_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_INCLUDE_DIRS})
endif()

# target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARIES} fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${RAYLIB_LIBRARIES}
    ${FMT_LIBRARIES}
    vk-bootstrap
    Vulkan::Vulkan
  )

#Copy shaders to putput dir when building
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert  $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/shader.vert
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag  $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/shader.frag
  COMMENT "Copying GLSL shaders to build output directory"
)
