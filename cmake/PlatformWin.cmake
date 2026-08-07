if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "x64 build currently is not supported. Please use Win32.")
endif()

set(RWDRV "d3d9" CACHE STRING "Using specific rw gx driver")
set_property(CACHE RWDRV PROPERTY STRINGS "opengl" "d3d9")

string(TOUPPER "${RWDRV}" RWDRV_UPPER)
target_compile_definitions(${EXEC_NAME} PRIVATE "${EXEC_NAME}_RWDRV_${RWDRV_UPPER}")

file_glob_dir_r(SRC_SYSTEM_WIN    "${DIR_SRC}/System/PC")
file_glob_dir_r(SRC_SOUND_BACKEND "${DIR_SRC}/System/Common/Sound/Backend/DSound")
file_glob_dir_r(SRC_SOUND_OS      "${DIR_SRC}/System/Common/Sound/Os/Win")

target_sources(${EXEC_NAME} PRIVATE 
    ${SRC_SYSTEM_WIN}
    ${SRC_SOUND_BACKEND} 
    ${SRC_SOUND_OS}
)

target_sources(${EXEC_NAME} PRIVATE 
    ${DIR_SRC}/System/PC/tmnt2.rc
)

get_target_property(ALL_FILES ${EXEC_NAME} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${ALL_FILES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXEC_NAME})

set_target_properties(${EXEC_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
set_target_properties(${EXEC_NAME} PROPERTIES LINK_FLAGS "/MACHINE:X86")

target_compile_definitions(${EXEC_NAME} PRIVATE 
    $<$<CONFIG:Debug>:
        RWDEBUG>
    _MBCS
    TARGET_PC 
    ${EXEC_NAME}_SOUND_DSOUND
)

target_precompile_headers(${EXEC_NAME} PRIVATE 
    "${DIR_SRC}/Game/pch.hpp"
)

target_include_directories(${EXEC_NAME} PRIVATE 
    "${DIR_SRC}"
    "${DIR_SRC}/System/Common/Sound"
    "${DIR_LIB}/cri/include"
    "${DIR_LIB}/${RWSDK}/include/${RWDRV}"
)

target_link_directories(${EXEC_NAME} PRIVATE 
    "$<$<CONFIG:Debug>:${DIR_LIB}/${RWSDK}/lib/${RWDRV}/debug>"
    "$<$<CONFIG:Release>:${DIR_LIB}/${RWSDK}/lib/${RWDRV}/release>"
    "$<$<CONFIG:RelMinSize>:${DIR_LIB}/${RWSDK}/lib/${RWDRV}/release>"
    "$<$<CONFIG:RelWithDebInfo>:${DIR_LIB}/${RWSDK}/lib/${RWDRV}/release>"
    "${DIR_LIB}/cri/lib/x86"
)

target_compile_options(${EXEC_NAME} PRIVATE 
    /MP
    /W4
    /GR-
    /wd4200 # nonstandard extension used: zero-sized array in struct/union
    /wd4201 # nonstandard extension used: nameless struct/union
    /wd4996 # function or variable may be unsafe
    /wd4100 # unreferenced formal parameter
    /wd4189 # local variable is initialized but not referenced
    /wd4505 # unreferenced local function has been removed
    /wd5054 # operator X deprecated between enumerations of different types
)

if(MSVC_VERSION GREATER_EQUAL 1910)
    target_compile_options(${EXEC_NAME} PRIVATE /permissive-)
    message(STATUS "Setting /permissive- for Visual Studio 2017+")
endif()

target_link_options(${EXEC_NAME} PRIVATE 
    $<$<CONFIG:Release>:
        /INCREMENTAL:NO>
    $<$<CONFIG:RelMinSize>:
        /INCREMENTAL:NO>
    /SAFESEH:NO 
    /DYNAMICBASE:NO 
    /MACHINE:X86
    /ignore:4099 # PDB 'X' was not found with 'X' or at 'X'; linking object as if no debug info
)
    
find_package(DXSDK REQUIRED)

target_link_libraries(${EXEC_NAME} PRIVATE 
    # win
    DXSDK::DXSDK
    winmm
    ksuser
    legacy_stdio_definitions

    # cri
    cri_adxpcx86_dsound8
    cri_mwsfdpcx86

    # rw
    rpskin
    rpmatfx
)

if(OPT_EU_BUILD)
    set(DEBUG_AFS_PATH "Z:/tmnt2_eu")
else()
    set(DEBUG_AFS_PATH "Z:/tmnt2_na")
endif()

set_target_properties(${EXEC_NAME} PROPERTIES 
    VS_DEBUGGER_COMMAND_ARGUMENTS "-afspath=${DEBUG_AFS_PATH} -wnd -lang=en"
)