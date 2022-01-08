set(CMAKE_CXX_STANDARD 17)

find_package(BISON 3.8 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

if (EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_BOOST_HEADERS=1 -fexceptions")
  add_compile_definitions(USE_LOG_STDIO)
endif (EMSCRIPTEN)

find_package(Boost 1.50.0 REQUIRED)
#  COMPONENTS system thread filesystem chrono timer)

#####

include(CheckTypeSize)
CHECK_TYPE_SIZE("char" SIZEOF_CHAR LANGUAGE CXX)
CHECK_TYPE_SIZE("short" SIZEOF_SHORT LANGUAGE CXX)
CHECK_TYPE_SIZE("int" SIZEOF_INT LANGUAGE CXX)
CHECK_TYPE_SIZE("long" SIZEOF_LONG LANGUAGE CXX)
CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG LANGUAGE CXX)
CHECK_TYPE_SIZE("float" SIZEOF_FLOAT LANGUAGE CXX)
CHECK_TYPE_SIZE("double" SIZEOF_DOUBLE LANGUAGE CXX)
CHECK_TYPE_SIZE("void*" SIZEOF_VOIDP LANGUAGE CXX)

include(TestBigEndian)
TEST_BIG_ENDIAN(IS_HOST_BIG_ENDIAN)
message(STATUS "IS_HOST_BIG_ENDIAN ${IS_HOST_BIG_ENDIAN}")

include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("stdarg.h" HAVE_STDARG_H)
CHECK_INCLUDE_FILE_CXX("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_CXX("locale.h" HAVE_LOCALE_H)
CHECK_INCLUDE_FILE_CXX("xlocale.h" HAVE_XLOCALE_H)
CHECK_INCLUDE_FILE_CXX("spawn.h" HAVE_SPAWN_H)
CHECK_INCLUDE_FILE_CXX("sys/wait.h" HAVE_SYS_WAIT_H)

include(CheckSymbolExists)
check_symbol_exists("vsnprintf" "stdio.h" HAVE_VSNPRINTF)
check_symbol_exists("vsnprintf_l" "xlocale.h" HAVE_VSNPRINTF_L)

if (${IS_HOST_BIG_ENDIAN})
  SET(BYTEORDER "4321")
else (${IS_HOST_BIG_ENDIAN})
  SET(BYTEORDER "1234")
endif (${IS_HOST_BIG_ENDIAN})

if (${Boost_FOUND})
  SET(HAVE_BOOST "1")
endif (${Boost_FOUND})

# if (${Boost_CHRONO_FOUND})
#   SET(HAVE_BOOST_CHRONO "1")
# endif (${Boost_CHRONO_FOUND})

# if (${Boost_FILESYSTEM_FOUND})
#   SET(HAVE_BOOST_FILESYSTEM "1")
# endif (${Boost_FILESYSTEM_FOUND})

if (${Boost_SYSTEM_FOUND})
  SET(HAVE_BOOST_SYSTEM "1")
endif (${Boost_SYSTEM_FOUND})

# if (${Boost_THREAD_FOUND})
#   SET(HAVE_BOOST_THREAD "1")
# endif (${Boost_THREAD_FOUND})

if (${Boost_TIMER_FOUND})
  SET(HAVE_BOOST_TIMER "1")
endif (${Boost_TIMER_FOUND})

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  add_definitions(-DBOOST_ALL_NO_LIB)
  add_definitions(-DBOOST_ALL_DYN_LINK)
endif()

#####
# Generate config.h

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_cmake.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
  @ONLY)
add_definitions(-DHAVE_CONFIG_H=1)

#####
# mcwrapgen setup
find_package(Perl REQUIRED)
include("mcwgen.cmake")

##########
# Subdirs

# support libraries
add_subdirectory(pcre)
add_subdirectory(expat)
add_subdirectory(zlib)
# add_subdirectory(libpng)

# main components/modules
add_subdirectory(qlib)
add_subdirectory(gfx)
add_subdirectory(qsys)

if (${BUILD_NODEJS_BINDINGS})
  add_subdirectory(node_jsbr)
endif()

if (${BUILD_EMSCRIPTEN_BINDINGS})
  add_subdirectory(embr)
endif()

# sub components/modules
add_subdirectory(modules/molstr)
