project(HardwareRenderEngine)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_FLAGS "-ggdb -Wall")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
set_property(GLOBAL APPEND PROPERTY COLLECT_LINK_LIBS "glfw3")
set_property(GLOBAL APPEND PROPERTY COLLECT_INC_DIR)

include(./buildTool/macro.cmake)
add_subdirectory(thirdPart)
add_subdirectory(renderCore)


add_executable(HardwareRenderEngine main.cpp)

ref_lib(renderCore)

link_lib_inc(HardwareRenderEngine "${LIBS}" "${INCS_OTHERS}")