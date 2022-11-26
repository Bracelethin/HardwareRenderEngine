
add_all_sources(./src)
set(OpenGL_GL_PREFERENCE LEGACY)
#find_package(glfw3 REQUIRED)
#find_package(OpenGL REQUIRED)
set(INCS ./include ./)
set(LIBS ${LIBS};GL;glfw)
common_collect(GLCollection)