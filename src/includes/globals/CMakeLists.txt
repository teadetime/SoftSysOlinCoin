message(STATUS "In Include Global")
set(INCLUDE_GLOBAL 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  CACHE INTERNAL "used internally")

set(INCLUDE_ALL 
  ${INCLUDE_ALL}
  ${CMAKE_CURRENT_SOURCE_DIR} 
  CACHE INTERNAL "used internally")