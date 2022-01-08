# Include node-addon-api wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
message(STATUS "NODE_ADDON_API_DIR: ${NODE_ADDON_API_DIR}")

# Essential include files to build a node addon,
# you should add this line in every CMake.js based project.

include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${Boost_INCLUDE_DIRS}
  ${NODE_ADDON_API_DIR}
  ${CMAKE_JS_INC}
  )

# Normal C++ source files
SET(NODE_JSBR_SRCS
  node_jsbr.cpp
  init_cuemol.cpp
  wrapper.cpp
  ElecView.cpp
  ElecDisplayContext.cpp
  ElecDisplayList.cpp
  ElecProgramObject.cpp
  ${CMAKE_JS_SRC}
  )

#####
# MCWRAPGEN
#

# CueMol interface definitions for MCWRAPGEN
SET(NODE_JSBR_MCWG_QIFS
  ElecView.qif
  )
# module loader for MCWRAPGEN
SET(NODE_JSBR_MODLDR_SRC node_jsbr.moddef)

MCWRAPGEN_CLASS(NODE_JSBR_SRCS ${NODE_JSBR_MCWG_QIFS})
MCWRAPGEN_MODULE(NODE_JSBR_SRCS ${NODE_JSBR_MODLDR_SRC} ${NODE_JSBR_MCWG_QIFS})

#####

if (WIN32)
  add_definitions(-DNODE_JSBR_EXPORTS=1)
endif()

SET(NODE_JSBR_LINK_LIBRARIES
  pcre
  expat
  # qmpng
  qmzlib
  qlib gfx qsys
  molstr
  ${CMAKE_JS_LIB}
  )

add_library(node_jsbr SHARED ${NODE_JSBR_SRCS} ${MCWG_HEADERS})
target_link_libraries(node_jsbr PRIVATE ${NODE_JSBR_LINK_LIBRARIES})

# This line will give our library file a .node extension without any "lib" prefix
set_target_properties(node_jsbr PROPERTIES PREFIX "" SUFFIX ".node")

# MCWRAPGEN_PYWRAPPERS(node_jsbr)
MCWRAPGEN_JS_WRAPPERS(node_jsbr)

install(TARGETS node_jsbr
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

SET(JS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/javascript)

add_custom_command(
  TARGET node_jsbr POST_BUILD
  COMMAND python3 ${CMAKE_SOURCE_DIR}/src/python/make_wrapper_table.py --input_dir ${CMAKE_BINARY_DIR}/javascript/wrappers --output_js ${JS_OUTPUT_DIR}/wrapper_loader.js
  )

add_custom_target(copy)

add_custom_command(
  TARGET copy PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/es6/*.js
  ${JS_OUTPUT_DIR}
  )

# XXX: copy to deploy dir
add_custom_command(
  TARGET copy PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*
  ${CMAKE_SOURCE_DIR}/src/data/shaders
  )

add_dependencies(node_jsbr copy)

# add_custom_command(
#   TARGET node_jsbr POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy
#   ${CMAKE_CURRENT_SOURCE_DIR}/es6/base_wrapper.js
#   ${JS_OUTPUT_DIR}/base_wrapper.js
#   )
