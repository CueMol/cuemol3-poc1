# cmake_minimum_required(VERSION 2.8)
cmake_minimum_required(VERSION 3.12)

if (COMMAND cmake_policy)
  # find_package() uses <PackageName>_ROOT variables.
  cmake_policy(SET CMP0074 NEW)
  # option() honors normal variables.
  cmake_policy(SET CMP0077 NEW)
endif ()

# Name of the project
project(CueMol3_poc CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Perl REQUIRED)
find_package(Python3 REQUIRED)

# if (APPLE)
#   set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "" FORCE)
# endif ()

option(BUILD_NODEJS_BINDINGS "Build node.js bindings" ON)
message(STATUS "BUILD_NODEJS_BINDINGS:" ${BUILD_NODEJS_BINDINGS})

# option(BUILD_PYTHON_BINDINGS "Build Python bindings")

if (EMSCRIPTEN)
  option(BUILD_EMSCRIPTEN_BINDINGS "Build emscripten bindings" ON)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/web_client/src/")
else ()
  option(BUILD_EMSCRIPTEN_BINDINGS "Build emscripten bindings" OFF)
endif()
message(STATUS "BUILD_EMSCRIPTEN_BINDINGS:" ${BUILD_EMSCRIPTEN_BINDINGS})

# main components/modules
add_subdirectory(src)

###########

message(STATUS
  "\n==============\n"
  "Summary report\n"
  "==============\n")

if (DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

message(STATUS "Build node.js bindings: ${BUILD_NODEJS_BINDINGS}")

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_VERSION})")
message(STATUS "Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
