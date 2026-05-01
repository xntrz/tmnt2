macro(file_glob OUT_VAR BASE_DIR IS_RECURSIVE)
    set(_EXTS "cpp" "hpp" "h" "c" "inl")
    set(_PATTERNS "")
    
    foreach(_E ${_EXTS})
        list(APPEND _PATTERNS "${BASE_DIR}/*.${_E}")
    endforeach()
  
    if(${IS_RECURSIVE})
        file(GLOB_RECURSE ${OUT_VAR} CONFIGURE_DEPENDS ${_PATTERNS})
    else()
        file(GLOB ${OUT_VAR} CONFIGURE_DEPENDS ${_PATTERNS})
    endif()
endmacro()

macro(file_glob_dir_r OUT_VAR BASE_DIR)
    file_glob(${OUT_VAR} "${BASE_DIR}" TRUE)
endmacro()

macro(file_glob_dir OUT_VAR BASE_DIR)
    file_glob(${OUT_VAR} "${BASE_DIR}" FALSE)
endmacro()

macro(print_list)
    string(REPLACE ";" "\n" _LIST "${ARGV}")
    message(STATUS "${_LIST}")
endmacro()