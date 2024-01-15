cmake_minimum_required(VERSION 3.10)

project(pbr)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-stdlib=libc++ -fexperimental-library -Wdeprecated")

aux_source_directory(src SRC)

include_directories(../filament/out/release/filament/include)

link_directories(../filament/out/release/filament/lib/x86_64 ../../../fmt/build/lib)

add_executable(demo ${SRC})

target_link_libraries(demo
                    #-static-libstdc++ -static-libgcc                     
                    filament 
                    filamat filagui filameshio gltfio_core gltfio                                    
                    filamentapp filamentapp-resources dracodec
                    shaders basis_transcoder geometry viewer  meshoptimizer                                
                    backend camutils ibl zstd  utils
                    filament-iblprefilter filabridge filaflat ktxreader bluegl bluevk vkshaders smol-v image viewer
                    filament
                    #filament-all filament filaflat gltfio gltfio_core utils
                    SDL2 stb imgui
                    )

install(TARGETS demo)

