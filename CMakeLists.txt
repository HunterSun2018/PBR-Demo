cmake_minimum_required(VERSION 3.10)

project(pbr)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-stdlib=libc++")

aux_source_directory(src SRC)

include_directories(../filament/release/include)

link_directories(../filament/release/lib/x86_64)

add_executable(demo ${SRC})

target_link_libraries(demo
                    -static-libstdc++ -static-libgcc     
                    filamentapp filamentapp-resources filament backend  filagui camutils ibl 
                    filament-iblprefilter filabridge filaflat ktxreader bluegl bluevk vkshaders smol-v image utils
                    sdl2 stb imgui
                    )

install(TARGETS demo)

