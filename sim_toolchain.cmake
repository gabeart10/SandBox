set(CMAKE_SYSTEM_NAME Linux)

add_compile_definitions(SIM_BUILD)

set(COMMON_FLAGS "-g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}" CACHE STRING "" FORCE)