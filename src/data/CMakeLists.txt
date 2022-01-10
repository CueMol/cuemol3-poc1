add_custom_target(data_copy ALL DEPENDS sysconfig.xml)

add_custom_command(
  OUTPUT sysconfig.xml
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/*
  ${CMAKE_BINARY_DIR}/data
  )
