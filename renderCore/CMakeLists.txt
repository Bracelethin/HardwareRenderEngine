set(INCS ./include)
set(INCS_OTHERS)
add_all_sources(./)

ref_lib(imgui GLCollection assimp)

common_collect(renderCore)

absolute_include_dirs(absPath "./" "../resources/")
list(POP_FRONT absPath tempPath)
add_compile_definitions(SHADER=\"${tempPath}\")
list(POP_FRONT absPath tempPath)
add_compile_definitions(RESOURCES=\"${tempPath}\")

