cmake_minimum_required(VERSION 3.26)

set(PROJ_NAME "TMNT2")
set(EXEC_NAME "TMNT2")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include("${CMAKE_SOURCE_DIR}/cmake/Macro.cmake")


#############################################
#                   Globals                 #
#############################################

project(${PROJ_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(DIR_ROOT "${CMAKE_SOURCE_DIR}")
set(DIR_SRC  "${DIR_ROOT}/src")
set(DIR_LIB  "${DIR_ROOT}/vendor")
set(DIR_DATA "${DIR_ROOT}/data")

if(WIN32 AND MSVC)
    set(TARGET_OS_NAME "win")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(EMSCRIPTEN)
    set(TARGET_OS_NAME "web")
else()
    message(FATAL_ERROR "Unexpected OS or Compiler: "
            "${CMAKE_SYSTEM_NAME} - ${CMAKE_CXX_COMPILER_ID}")
endif()

set(RWSDK "rwsdk37/${TARGET_OS_NAME}") 


#############################################
#              Common options               #
#############################################

option(OPT_EU_BUILD 
    "Build game as EU version" OFF
)

option(OPT_TEST 
    "Enable diag features mostly for release build like FPS & MEM counter" OFF
)

# TODO
#option(OPT_IDFSYST 
#    "Enables file access by ids instead file names" OFF
#)

option(OPT_TRIAL
    "Build as trial version" OFF
)

set(OPT_BIN_DIR "" CACHE STRING "Bin dir add path")


#############################################
#               Target common               #
#############################################

if(ANDROID)
    add_library(${EXEC_NAME} SHARED)
else()
    add_executable(${EXEC_NAME} "")
endif()

# get all "src/Game"
file_glob_dir_r(SRC_GAME "${DIR_SRC}/Game")

file_glob_dir_r(SRC_GAME_TEST_REMOVE "${DIR_SRC}/Game/Sequence/Test")
if(SRC_GAME_TEST_REMOVE)
    set_source_files_properties(${SRC_GAME_TEST_REMOVE} PROPERTIES 
        EXCLUDE_FROM_DEFAULT_BUILD_RELEASE          TRUE
        EXCLUDE_FROM_DEFAULT_BUILD_RELWITHDEBINFO   TRUE
        EXCLUDE_FROM_DEFAULT_BUILD_MINSIZEREL       TRUE
        #SKIP_PRECOMPILE_HEADERS                     TRUE 
    )
endif()

# get all "src/System" except sound platform specific - we will include it later
file_glob_dir_r(SRC_SYSTEM_COMMON "${DIR_SRC}/System/Common")
file_glob_dir_r(SRC_SOUND_REMOVE "${DIR_SRC}/System/Common/Sound")
list(REMOVE_ITEM SRC_SYSTEM_COMMON ${SRC_SOUND_REMOVE})
file_glob_dir(SRC_SYSTEM_SOUND "${DIR_SRC}/System/Common/Sound")

# now include all base
target_sources(${EXEC_NAME} PRIVATE 
    ${SRC_GAME}
    ${SRC_SYSTEM_COMMON}
    ${SRC_SYSTEM_SOUND}
)

# base libs 
# (rpskin & rpmatfx should be specified by platform specific)
target_link_libraries(${EXEC_NAME} PRIVATE 
    # rw: core -> plugins -> tools
    rwcore
    rpcollis
    rphanim
    rplodatm
    #rplogo (TODO: eval logo for web)
    rppvs
    rprandom
    #rpskin (or rpskintoon)
    #rpmatfx (or rpskinmatfxtoon)
    rptoon
    rpuvanim
    rpworld
    rt2d
    rt2danim
    rtanim
    rtbmp
    rtcharse
    rtdict
    rtfsyst
    rtintsec
    rtpitexd
    rtpng
    rtquat
)

# base defines
if(OPT_EU_BUILD)
    target_compile_definitions(${EXEC_NAME} PRIVATE ${EXEC_NAME}_BUILD_EU)
endif()

if(OPT_TEST)
    target_compile_definitions(${EXEC_NAME} PRIVATE ${EXEC_NAME}_TEST)
endif()

if(OPT_IDFSYST)
    # TODO
    # target_compile_definitions(${EXEC_NAME} PRIVATE ${EXEC_NAME}_IDFSYST)
endif()

if(OPT_TRIAL)
    target_compile_definitions(${EXEC_NAME} PRIVATE ${EXEC_NAME}_TRIAL)
endif()


#############################################
#           Target platform specific        #
#############################################

if(WIN32 AND MSVC)
    include(PlatformWin)
elseif(EMSCRIPTEN)
    include(PlatformWeb)
else()
    message(FATAL_ERROR "Unexpected platform: ${CMAKE_SYSTEM_NAME}")
endif()

foreach(CONFIG DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    string(TOLOWER "${CONFIG}" CONFIG_LOWER)
    set_target_properties(${EXEC_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${DIR_ROOT}/bin/${TARGET_OS_NAME}/${RWDRV}/${CONFIG_LOWER}/${OPT_BIN_DIR}"
    )
endforeach()
