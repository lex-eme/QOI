cmake_minimum_required(VERSION 3.28)
project(QOI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp
        src/qoi.cpp
        src/qoi.h)

add_library(profiling profiling/Profiler.cpp
        profiling/Profiler.h
        profiling/ProfileTimer.cpp
        profiling/ProfileTimer.h)

target_include_directories(main PUBLIC profiling)

target_link_libraries(main PRIVATE sfml-graphics profiling)
#target_compile_features(main PRIVATE cxx_std_23)
#target_compile_features(profiling PRIVATE cxx_std_23)

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()
