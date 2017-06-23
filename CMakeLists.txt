cmake_minimum_required(VERSION 2.8)

project(CPP1)

set(PROJECT_DIR ${PROJECT_SOURCE_DIR})

message(----------------------------------------------------------------------------------)

# After you having compiled your sdl and ffmpeg binaries,  you can put at path relate your project path.
# WhereIsFFMPEGLibRoot represent ffmpeg-dev root path.
# WhereIsSDLLibRoot sdl-dev root path.
# You have to use relative path to specify where they are.
# This script is tested on Ubuntu used CMake 2.8

# It's not recommend to use MinGW on Windows. Because MinGW's make will crash in unknown issue when you build it.

set(WhereIsFFMPEGLibRoot ../ffmpeg-dev)
set(WhereIsSDLLibRoot ../SDL-dev)

set(FFMpegIncludeHeaders ${PROJECT_DIR}/${WhereIsFFMPEGLibRoot}/include)
set(FFMpegLibraryFiles ${PROJECT_DIR}/${WhereIsFFMPEGLibRoot}/lib)
set(SDLIncludeHeaders ${PROJECT_DIR}/${WhereIsSDLLibRoot}/include)
set(SDLLibraryFiles ${PROJECT_DIR}/${WhereIsSDLLibRoot}/lib)


include_directories( ${FFMpegIncludeHeaders})
include_directories( ${FFMpegLibraryFiles})
include_directories( ${SDLIncludeHeaders})
include_directories( ${SDLLibraryFiles})
#-I${FFMpegIncludeHeaders}   -L${FFMpegLibraryFiles} -mwindows -Wall -D__WIN32__
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g  -O2  -I${SDLIncludeHeaders}  -I${FFMpegIncludeHeaders} -L${FFMpegLibraryFiles} -L${SDLLibraryFiles}  -lSDLmain -lSDL  -lavcodec")

#message(${CMAKE_C_FLAGS})

message(Project Locating At: ${PROJECT_DIR})
message(FFMPEG Headers Locating At: ${FFMpegIncludeHeaders})
message(FFMPEG Library Locating At: ${FFMpegLibraryFiles})
message(SDL Headers Locating At: ${SDLIncludeHeaders})
message(SDL Library Locating At: ${SDLLibraryFiles})


add_library(libSDL2 SHARED IMPORTED)
set_target_properties(libSDL2 PROPERTIES IMPORTED_LOCATION ${SDLLibraryFiles}/libSDL2.a)

add_library(libSDL2main SHARED IMPORTED)
set_target_properties(libSDL2main PROPERTIES IMPORTED_LOCATION ${SDLLibraryFiles}/libSDL2main.a)

add_library(libavcodec SHARED IMPORTED)
set_target_properties(libavcodec PROPERTIES IMPORTED_LOCATION ${FFMpegLibraryFiles}/libavcodec.a)

add_library(libavdevice SHARED IMPORTED)
set_target_properties(libavdevice PROPERTIES IMPORTED_LOCATION ${FFMpegLibraryFiles}/libavdevice.a)

add_library(libavfilter SHARED IMPORTED)
set_target_properties(libavfilter PROPERTIES IMPORTED_LOCATION ${FFMpegLibraryFiles}/libavfilter.a)

add_library(libavformat SHARED IMPORTED)
set_target_properties(libavformat PROPERTIES IMPORTED_LOCATION ${FFMpegLibraryFiles}/libavformat.a)

add_library(libavutil SHARED IMPORTED)
set_target_properties(libavutil PROPERTIES IMPORTED_LOCATION ${FFMpegLibraryFiles}/libavutil.a)

add_library(libswresample SHARED IMPORTED)
set_target_properties(libswresample PROPERTIES IMPORTED_LOCATION ${FFMpegLibraryFiles}/libswresample.a)

add_library(libswscale SHARED IMPORTED)
set_target_properties(libswscale PROPERTIES IMPORTED_LOCATION ${FFMpegLibraryFiles}/libswscale.a)

set(CMainSources  "main.c")

add_executable(helloFFmpeg ${CMainSources})
target_link_libraries(helloFFmpeg libSDL2 libSDL2main libavcodec libavdevice libavfilter libavformat  libswresample libswscale libavutil  m pthread dl z lzma )
message(----------------------------------------------------------------------------------)
