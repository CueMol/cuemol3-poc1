include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${Boost_INCLUDE_DIRS}
  )

# Normal C++ source files
SET(EMBR_SRCS
  embr.cpp
  init_cuemol.cpp
  direct.cpp
  EmView.cpp
  EmDisplayContext.cpp
  EmProgramObject.cpp
  EmDisplayList.cpp
  EmShaderObject.cpp
  EmProgObjMgr.cpp
)

#####
# MCWRAPGEN
#

# CueMol interface definitions for MCWRAPGEN
SET(EMBR_MCWG_QIFS
  EmView.qif
  )
# module loader for MCWRAPGEN
SET(EMBR_MODLDR_SRC embr.moddef)

MCWRAPGEN_CLASS(EMBR_SRCS ${EMBR_MCWG_QIFS})
MCWRAPGEN_MODULE(EMBR_SRCS ${EMBR_MODLDR_SRC} ${EMBR_MCWG_QIFS})

#####

SET(EMBR_LINK_LIBRARIES
  pcre
  expat
  # qmpng
  qmzlib
  qlib gfx qsys
  molstr
  )

# SET(EMCC_PRE_JS "--pre-js ${CMAKE_CURRENT_SOURCE_DIR}/pre.js")
SET(EMCC_PRE_JS
  "--preload-file ${CMAKE_SOURCE_DIR}/data@/ --preload-file ${CMAKE_SOURCE_DIR}/shaders@/shaders"
)

add_executable(embr ${EMBR_SRCS} ${MCWG_HEADERS})
target_link_libraries(embr PRIVATE ${EMBR_LINK_LIBRARIES})

set_target_properties(embr PROPERTIES LINK_FLAGS "--no-entry -s USE_WEBGL2=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s EXPORT_ES6=1 -s MODULARIZE=1 -s USE_BOOST_HEADERS=1 -fexceptions ${EMCC_PRE_JS} -s EXPORTED_RUNTIME_METHODS=intArrayFromString,allocate,ALLOC_NORMAL,UTF8ToString")
#
#
MCWRAPGEN_JS_WRAPPERS(embr)

add_custom_command(
  TARGET embr POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/es6/cuemol.js
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libs/cuemol.js
  )

add_custom_command(
  TARGET embr POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/es6/base_wrapper.js
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libs/base_wrapper.js
  )

add_custom_command(
  TARGET embr POST_BUILD
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/make_wrapper_table.py --input_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libs/wrappers/ --output_js ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libs/wrapper_loader.js
#  COMMAND ${CMAKE_COMMAND} -E copy
#  ${CMAKE_CURRENT_SOURCE_DIR}/es6/wrapper_loader.js
#  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libs/wrapper_loader.js
  )
