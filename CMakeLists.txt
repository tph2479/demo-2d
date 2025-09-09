cmake_minimum_required(VERSION 3.10.0)
project(Proj2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src/include)
# set(LIBJPEG_LIB_PATH "${CMAKE_SOURCE_DIR}/vendor/libjpeg/lib/libturbojpeg.a")

# file(GLOB_RECURSE SOURCES "src/*.cpp")
# set(SOURCES
#     src/main.cpp
# )

function(add target_name source_file)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_executable(${target_name} WIN32 ${source_file})
        file(COPY ${CMAKE_SOURCE_DIR}/src/images DESTINATION ${CMAKE_BINARY_DIR}/release)
    else()
        add_executable(${target_name} ${source_file})
        file(COPY ${CMAKE_SOURCE_DIR}/src/images DESTINATION ${CMAKE_BINARY_DIR}/debug)
    endif()
    
    if(ARGN)
        target_link_libraries(${target_name} ${ARGN})
    endif()
    

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release)
        # file(COPY ${CMAKE_SOURCE_DIR}/src/images DESTINATION ${CMAKE_BINARY_DIR}/release)
    else()
        set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)
        file(COPY ${CMAKE_SOURCE_DIR}/src/images DESTINATION ${CMAKE_BINARY_DIR}/debug)
    endif()
endfunction()

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

add(font src/font.cpp
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add(cube src/cube.cpp
        SDL3::SDL3-static
    )
    add(cute src/cute.cpp
        SDL3::SDL3-static
    )
else()
    add(cube src/cube.cpp
        SDL3::SDL3
    )
    add(cute src/cute.cpp
        SDL3::SDL3
    )
endif()

add(click src/click_effect.cpp
    SDL3::SDL3
)

add(loading src/loading.cpp
    SDL3::SDL3
)

# add_sdl_executable(Proj2 src/main.cpp)

# # Optional: Set compiler optimizations
# if(CMAKE_BUILD_TYPE STREQUAL "Release")
#     target_compile_options(Proj2 PRIVATE -O3 -DNDEBUG)
# else()
#     target_compile_options(Proj2 PRIVATE -g -O0)
# endif()