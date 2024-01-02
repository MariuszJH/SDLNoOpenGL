cmake_minimum_required(VERSION 3.26.0...3.26.3)

include(CMakeUtils/Handy/DisallowInSourceBuilds.cmake) # Comment out to allow
include(CMakeUtils/Handy/SelectCompilerViaFlagIfNotUsingCMakePresets.cmake)  # or else default compiler will be used

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
project(SDLNoOpenGL VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

include(CMakeUtils/Handy/DisplayGenericInfo.cmake) # Comment out to suppress

#[==================================================[
SDLNoOpenGL (executable program)
#]==================================================]
add_executable(targetExe) 

target_include_directories(targetExe
    PRIVATE
        SDLNoOpenGL/hdr
)

target_sources(targetExe
    PRIVATE
        SDLNoOpenGL/src/MainSDLNoOpenGL.cpp
        SDLNoOpenGL/hdr/SDLWindowNoOpenGL.hpp
        SDLNoOpenGL/src/SDLWindowNoOpenGL.cpp
)

set_target_properties(targetExe
    PROPERTIES
        OUTPUT_NAME                         SDLNoOpenGL
        DEBUG_POSTFIX                       "_d"
        RUNTIME_OUTPUT_DIRECTORY            ${CMAKE_BINARY_DIR}/buildOut/SDLNoOpenGL/bin/$<CONFIG>
        PDB_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/buildOut/SDLNoOpenGL/pdb/$<CONFIG>
        RUNTIME_OUTPUT_DIRECTORY_<Config>   ${RUNTIME_OUTPUT_DIRECTORY}
        PDB_OUTPUT_DIRECTORY_<Config>       ${PDB_OUTPUT_DIRECTORY}
)

target_compile_features(targetExe
    PRIVATE
        c_std_17
        cxx_std_20
)

if(MSVC)

    target_compile_options(targetExe 
        PRIVATE
            /EHsc
            /W2
            # /Wall
    )

else()

    target_compile_options(targetExe 
        PRIVATE
            -Wall
            # -Wextra
            # -Wpedantic
    )

endif()

#[==================================================[
Find or fetch a library and display its cmake targets
#]==================================================]
include(CMakeUtils/FindOrFetch/FindOrFetchSDL2.cmake)

get_property(importTargets1 DIRECTORY ${SOURCE_DIR} PROPERTY IMPORTED_TARGETS)
message(STATUS "importTargets1: ${importTargets1}")

#[==================================================[
On Windows copy *.dll needed at runtime by the .exe into the same directory as the .exe
#]==================================================]
option(isThereDllToLink "Switch OFF when not linking any DLLs, otherwise copy <empty> will fail!" ON)

if(WIN32 AND isThereDllToLink)
    add_custom_command(TARGET targetExe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "copy: $<TARGET_RUNTIME_DLLS:targetExe>"
        COMMAND ${CMAKE_COMMAND} -E echo "to:   $<TARGET_FILE_DIR:targetExe>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                $<TARGET_RUNTIME_DLLS:targetExe> 
                $<TARGET_FILE_DIR:targetExe>
        COMMAND_EXPAND_LISTS
    )
endif()

#[==================================================[
Link executable with found/fetched libraries
#]==================================================]
target_link_libraries(targetExe 
    PRIVATE 
        SDL2::SDL2
        SDL2::SDL2main
)

#[==================================================[
Run the executable 
#]==================================================]
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E echo "execute: $<TARGET_FILE:targetExe>"
    COMMAND $<TARGET_FILE:targetExe>
)

#[==================================================[
(Optional) IDE solution name
Must be after all other projects since last project(...) is used as solution name
#]==================================================]
if (CMAKE_GENERATOR MATCHES "Visual Studio" OR  # any VS generator
    CMAKE_GENERATOR MATCHES "Eclipse CDT4"  OR  # any Eclipse generator
    CMAKE_GENERATOR MATCHES "Xcode")

    project(SDLNoOpenGLExample)

endif()
