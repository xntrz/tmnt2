option(OPT_CLOSURE_COMPILER
    "Invoke closure compiler" OFF
)

option(OPT_MULTI
    "Specifies this is multi versions build for web (eu, na, trial)" OFF
)

set(RWDRV "opengl")
set(SHELL "index")

file_glob_dir_r(SRC_SYSTEM_WEB    "${DIR_SRC}/System/Web")
file_glob_dir_r(SRC_SOUND_BACKEND "${DIR_SRC}/System/Common/Sound/Backend/OpenAL")
file_glob_dir_r(SRC_SOUND_OS      "${DIR_SRC}/System/Common/Sound/Os/Web")

target_sources(${EXEC_NAME} PRIVATE 
    ${SRC_SYSTEM_WEB}
    ${SRC_SOUND_BACKEND} 
    ${SRC_SOUND_OS}
)

set_target_properties(${EXEC_NAME} PROPERTIES OUTPUT_NAME ${SHELL})
set_target_properties(${EXEC_NAME} PROPERTIES SUFFIX      ".js")

target_compile_definitions(${EXEC_NAME} PRIVATE 
    $<$<CONFIG:Debug>:_DEBUG>
    TARGET_WEB
    ${EXEC_NAME}_RWDRV_OPENGL
    ${EXEC_NAME}_SOUND_OPENAL
)

target_precompile_headers(${EXEC_NAME} PRIVATE 
    "${DIR_SRC}/Game/pch.hpp"
)

target_include_directories(${EXEC_NAME} PRIVATE 
    "${DIR_SRC}"
    "${DIR_SRC}/System/Common/Sound"
    "${DIR_SRC}/System/Web/Cri"
    "${DIR_LIB}/${RWSDK}/include/${RWDRV}"
    "${DIR_LIB}/sdl3/${TARGET_OS_NAME}/include"
    "${DIR_LIB}/gl4es/${TARGET_OS_NAME}/include"
)

target_link_directories(${EXEC_NAME} PRIVATE 
    "$<$<CONFIG:Debug>:${DIR_LIB}/${RWSDK}/lib/${RWDRV}/release>"
    "$<$<CONFIG:Release>:${DIR_LIB}/${RWSDK}/lib/${RWDRV}/release>"
    "$<$<CONFIG:RelMinSize>:${DIR_LIB}/${RWSDK}/lib/${RWDRV}/release>"
    "$<$<CONFIG:RelWithDebInfo>:${DIR_LIB}/${RWSDK}/lib/${RWDRV}/release>"
    "${DIR_LIB}/sdl3/${TARGET_OS_NAME}/lib"
    "${DIR_LIB}/gl4es/${TARGET_OS_NAME}/lib"
)

#
# DOCS: https://emscripten.org/docs/tools_reference/settings_reference.html
#       https://emscripten.org/docs/api_reference/module.html
#

set(COMPILE_OP_WOFF
    -Wno-gnu-empty-struct
    -Wno-dollar-in-identifier-extension
    -Wno-reorder-ctor
    -Wno-ignored-qualifiers
    -Wno-zero-length-array
    -Wno-extra-semi
    -Wno-enum-compare
    -Wno-overloaded-virtual
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-const-variable
    -Wno-unused-but-set-variable
    -Wno-unused-private-field
    -Wno-invalid-utf8
    -Wno-gnu-zero-variadic-macro-arguments
    -Wno-keyword-macro
    -Wno-c99-extensions
    -Wno-unneeded-internal-declaration
)

set(LINK_OP_MEM
    -sINITIAL_MEMORY=64MB
    #-sALLOW_MEMORY_GROWTH=1
    -sSTACK_SIZE=2MB
    #-sSTACK_OVERFLOW_CHECK=2
)

# init preload files
set(LINK_OP_PRELOAD "")

if(OPT_EU_BUILD)
    set(PRELOADFILE_FONT_MET ${DIR_DATA}/Common/Fonts/MainFont.met_eu)
else()
    set(PRELOADFILE_FONT_MET ${DIR_DATA}/Common/Fonts/MainFont.met_na)
endif()

list(APPEND LINK_OP_PRELOAD 
    --preload-file=${PRELOADFILE_FONT_MET}@Common/Fonts/MainFont.met
)

function(web_preload_dir dir_path out_variable)
    file(GLOB files_list "${dir_path}/*")
    set(preload_flags "")
    foreach(full_path ${files_list})
        if(NOT IS_DIRECTORY "${full_path}")
            get_filename_component(filename "${full_path}" NAME)
            list(APPEND preload_flags "--preload-file=${full_path}@${filename}")
        endif()
    endforeach()
    set(${out_variable} "${preload_flags}" PARENT_SCOPE)
endfunction()

if(OPT_TRIAL)
    list(APPEND LINK_OP_PRELOAD 
        --preload-file=${DIR_DATA}/Common/Trial/Trial.lpac@Common/Trial/Trial.lpac)
    
    list(APPEND LINK_OP_PRELOAD 
        --preload-file=${DIR_DATA}/Common/Menu/Logo/LogoTR.lpac@Common/Menu/Logo/LogoTR.lpac)

    web_preload_dir("${DIR_DATA}/trial" PRELOADFILE_TRIAL)
    #print_list(${PRELOADFILE_TRIAL})
    list(APPEND LINK_OP_PRELOAD "${PRELOADFILE_TRIAL}")
endif()

# compile & link shared flags
set(COMPILE_LINK_SHARED_OP
    -fno-rtti
    -fno-exceptions
    -msimd128
    -flto
    -ffast-math
    -fno-math-errno
    -funsafe-math-optimizations
    -freciprocal-math
    -fno-trapping-math
    #-g
)

if(OPT_CLOSURE_COMPILER)
    set(LINK_OP_CLOSURE
        --closure 1
        --closure-externs
        --closure-args="--externs=${DIR_SRC}/System/Web/externs.js"
    )
endif()

target_compile_options(${EXEC_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -fvisibility=hidden
    ${COMPILE_OP_WOFF}
    ${COMPILE_LINK_SHARED_OP}
)

target_link_options(${EXEC_NAME} PRIVATE
    -sFULL_ES2=1 # required for gl4es
    -sWASM=1
    -sASYNCIFY=1
    -sASYNCIFY_ONLY=@${DIR_SRC}/System/Web/asyncify_only.txt
    -sEXPORTED_RUNTIME_METHODS=@${DIR_SRC}/System/Web/export_runtime.txt
    -sEXPORTED_FUNCTIONS=@${DIR_SRC}/System/Web/export_functions.txt
    -sASSERTIONS=0
    -sFETCH
    -sMALLOC=emmalloc
    -sFILESYSTEM=1
    -sFORCE_FILESYSTEM=1
    -sMINIFY_HTML=0
    -sENVIRONMENT=web,worker
    -sEXIT_RUNTIME=0
    --gc-sections
    ${LINK_OP_MEM}
    ${COMPILE_LINK_SHARED_OP}
    ${LINK_OP_CLOSURE}
    ${LINK_OP_PRELOAD}
)

target_link_libraries(${EXEC_NAME} PRIVATE 
    # proj
    "${DIR_LIB}/gl4es/${TARGET_OS_NAME}/lib/libGL.a"
    SDL3

    # ems
    memfs.js
    idbfs.js
    workerfs.js

    # rw
    rpmatfx
    rpskintoon
    rpskinmatfxtoon
)

# preprocess .html file
set(CMAKE_HTML_IS_MULTI_BUILD false)
set(CMAKE_HTML_IS_TRIAL_BUILD false)

if(OPT_MULTI)
    set(CMAKE_HTML_IS_MULTI_BUILD true)
endif()

if(OPT_TRIAL)
    set(CMAKE_HTML_IS_TRIAL_BUILD true)
endif()

configure_file(
    "${DIR_SRC}/System/Web/${SHELL}.html.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${SHELL}.html"
    @ONLY
)

# copy web assets to bin dir
file(GLOB_RECURSE WEB_FILES 
    "${DIR_SRC}/System/Web/*.js"
    "${DIR_SRC}/System/Web/*.json"
    "${DIR_SRC}/System/Web/*.png"
    "${DIR_SRC}/System/Web/*.ico"
    "${CMAKE_CURRENT_BINARY_DIR}/${SHELL}.html"
)

add_custom_target(copy_web_assets
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${EXEC_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WEB_FILES} "$<TARGET_FILE_DIR:${EXEC_NAME}>"
    DEPENDS ${WEB_FILES}
    COMMENT "Copy web assets..."
)

add_dependencies(${EXEC_NAME} copy_web_assets)