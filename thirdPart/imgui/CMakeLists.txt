set(INCS ./ ./backends)

add_all_sources(./)
list(APPEND SRCS ./backends/imgui_impl_glfw.cpp ./backends/imgui_impl_opengl3.cpp)


common_collect(imgui)