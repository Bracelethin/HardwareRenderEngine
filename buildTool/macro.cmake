
function(absolute_include_dirs
  includes_absolute)

  set(_ALL_INCS "")
  foreach(_INC ${ARGN})
    get_filename_component(_ABS_INC ${_INC} ABSOLUTE)
    list(APPEND _ALL_INCS ${_ABS_INC})
    # for checking for invalid includes, disable for regular use
     if(NOT EXISTS "${_ABS_INC}/")
       message(FATAL_ERROR "Include not found: ${_ABS_INC}/")
     endif()
  endforeach()

  set(${includes_absolute} ${_ALL_INCS} PARENT_SCOPE)
endfunction()

macro(link_lib_inc libName libs incs_others)
    target_include_directories(${libName} PUBLIC ${incs_others})

    target_link_libraries(${libName} PUBLIC ${libs})
endmacro()


macro(cus_add_lib libName srcs libs incs incs_others)
    absolute_include_dirs(ab_incs ${incs})
    foreach(inc ${ab_incs})
        include_directories(${inc})
        set_property(GLOBAL APPEND PROPERTY ${libName}_INC_PATH ${inc})
    endforeach()

    add_library(${libName} SHARED ${srcs})

    link_lib_inc(${libName} "${libs}" "${incs_others}")
    
    set_property(GLOBAL APPEND PROPERTY COLLECT_LINK_LIBS ${libName})
endmacro()

macro(common_collect libName)
    cus_add_lib("${libName}" "${SRCS}" "${LIBS}" "${INCS}" "${INCS_OTHERS}")
endmacro()

macro(collect_all_file file_path container pattern)
    file(GLOB ${container} ${file_path}/${pattern})
endmacro()

function(add_all_sources file_path)
    collect_all_file(${file_path} SRCS1 "*.cpp")
    collect_all_file(${file_path} SRCS2 "*.c")
    set(SRCS ${SRCS1} ${SRCS2} PARENT_SCOPE)
#    message(${SRCS})
endfunction()

macro(ref_lib)
    foreach(lib ${ARGV})
        get_property(incs GLOBAL PROPERTY ${lib}_INC_PATH)
        set(INCS_OTHERS ${INCS_OTHERS};${incs})
        set(LIBS ${LIBS};${lib})
    endforeach()
endmacro()