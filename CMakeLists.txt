cmake_minimum_required(VERSION 3.15)
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)
set(APPEND PROJECT_NAME ${PROJECT_NAME})
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(_VULKAN_MSYS2_PREFIX "C:/msys64/mingw64" CACHE PATH "msys2 MinGW64 prefix for Vulkan")
list(APPEND CMAKE_PREFIX_PATH "C:/msys64/mingw64")

#Find libaries
find_package(raylib CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(vk-bootstrap REQUIRED)
find_package(Vulkan REQUIRED)
# if(NOT glfw_FOUND)
find_package(glfw QUIET)
# endif()
#==========================================
if(fmt_FOUND)
   message(STATUS "Found fmt via Cmake CONFIG")
   set(FMT_LIBRARIES fmt)
else()
  message(STATUS "fmt ot found on system, we well intal it")
  message(STATUS "we well install fmt library now! ❣️")
  find_library(FMT_LIBRARIES NAMES fmt PATHS "C:/msys64/mingw64/lib")
  find_path(FMT_LIBRARIES NAMES fmt.h PATHS "C:/msys64/mingw64/include")
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG  11.0.0
    FIND_PACKAGE_ARGS
  )
  FetchContent_MakeAvailable(fmt)
endif()

# #===========================================================================
#first check system/installed packages
#if not found on system , install it
if(raylib_FOUND)
  message(STATUS "Found raylib via Cmake CONFIG")
  set(RAYLIB_LIBRARIS raylib)
else()
  #Fallback to manual discovery
  find_library(RAYIB_LIBRARIES NAMES raylib PATHS "C:/msys64/mingw64/lib")
  find_path(RAYLIB_INCLUDE_DIRS NAMES raylib.h PATHS "C:/msys64/mingw64/include")
  if(RAYLIB_LIBRARIS AND RAYLIB_INCLUDE_DIRS)
    set(RAYLIB_libraris raylib)
    #fallback to manual discovery
    find_library(RAYLIB_LIBRARIS names raylib PATHS "C:/msys64/mingw64/lib")
    find_path(RAYLIB_LIBRARIS NAMES raylib PATHS "C:/msys64/mingw64/include")
    message(STATUS "Found raylib manually: ${RAYLIB_LIBRARIES}")
  else()
    message(FATAL_ERROR "Could Not find raylib library of headrs!")
  endif()
endif()

if(vk-bootstrap_FOUND)
  message(STATUS "Found vk-bootstrap via Cmake CONFIG 🎉")
  set(VK_BOOSTRAP_LIBRARIES vk-bootstrap)
else()
  message(FATAL_ERROR "Could not find VkBoostrap library")
  find_library(VK_BOOSTRAP_LIBRARIES NAMES vk-bootstrap PATH "C:/msys64/mingw64/lib")
  find_path(VK_BOOSTRAP_LIBRARIES NAMES vk-bootstrap PATHS "C:/msys64/mingw64/include")

  FetchContent_Declare(VkBoostrap
    GIT_REPOSITORY https://github.com/charles-lunarg/VkBoostrap
    GIT_TAG v1.3.290
  )
  FetchContent_MakeAvailable(vk-bootstrap)
endif()

if(glfw_FOUND)
  message(STATUS "Found glfw via Cmake CONFIG ⭐")
  set(GLFW_LIBRARIES glfw)
  set(GLFW_INCLUDE_DIRS  "${CMAKE_PREFIX_PATH}/include")
else()
  # message(FATAL_ERROR "Could not find GLFW library")
  message(STATUS "glfw insalling via FetchContent 🔧")
  find_library(GLFW NAMES glfw  PATHS "C:/msys64/mingw64/lib")
  find_path(GLFW NAMES GLFW/glfw3.h  PATHS "C:/msys64/mingw64/include")

  if(GLFW_LIBRARIES AND   GLFW_INCLUDE_DIRS)
    set(GLFW_LIBRARIES glfw)
    set(GLFW_INCLUDE_DIRS "${GLFW_INCLUDE_DIRS}")
    message(STATUS "Found glfw manually: ${GLFW_LIBRARIES}")
  else()
    message(STATUS "Insalling glfw from source...")
    FetchContent_Declare(
      glfw
      GIT_REPOSITORY https://github.com/glfw/glfw.git
      GIT_TAG 3.4
    )
    #Prevent glfw from trying to build its own examples/test
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DIRS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
    set(GLFW_LIBRARIES glfw)
    set(GLFW_INCLUDE_DIRS "${glfw_SOURCE_DIR}/include")
  endif()
endif()



#Executable
add_executable(${PROJECT_NAME} src/main.cpp

)

# add_executable(${PROJECT_NAME} src/main.cpp
#   src/Enemy.cpp
#   src/Game.cpp
#   src/Player.cpp
#   src/Bullet.cpp
#   src/Vulkancompute.cpp
# )
if(RAYLIB_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME} PRIVATE ${RAYLIB_INCLUDE_DIRS})
endif()

# target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARIES} fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${RAYLIB_LIBRARIS}
  ${FMT_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${VK_BOOSTRAP_LIBRARIES}
  Vulkan::Vulkan
)

#Copy shaders to put dir when building
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert  $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/shader.vert
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag  $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/shader.frag
  COMMENT "Copying GLSL shaders to build output directory"
)
