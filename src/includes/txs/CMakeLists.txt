message(STATUS "In Include TX")
set(INCLUDE_TX 
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "used internally")
set(INCLUDE_ALL 
  ${INCLUDE_ALL}
  ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE INTERNAL "used internally")
