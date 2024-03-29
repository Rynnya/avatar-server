cmake_minimum_required(VERSION 3.1)
set(project_name avatar-server)
project(${project_name})

set(OATPP_BUILD_TESTS OFF CACHE BOOL "Create test target for oat++")
set(OATPP_DISABLE_ENV_OBJECT_COUNTERS ON CACHE BOOL "Do not count oatpp objects")

set(CMAKE_CXX_STANDARD 17)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/libs/oatpp/CMakeLists.txt)
    message(FATAL_ERROR "oatpp is missing in folder /libs/oatpp. Have you initialized the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/libs/redis/CMakeLists.txt)
    message(FATAL_ERROR "cpp_redis is missing in folder /libs/redis. Have you initialized the submodules / downloaded the extra libraries?")
endif()

add_subdirectory(libs/oatpp/)
add_subdirectory(libs/redis/)
include_directories(src)

add_executable(${project_name}
    src/components.hpp
    src/concurrent_cache.hpp
    src/controller.cpp
    src/controller.hpp
    src/globals.cpp
    src/globals.hpp
    src/main.cpp
)

## link libs
target_link_libraries(${project_name} PUBLIC oatpp cpp_redis)

target_include_directories(${project_name} PUBLIC src)
target_include_directories(${project_name} PRIVATE ${CMAKE_SOURCE_DIR}/libs/redis/includes)

set_target_properties(${project_name} PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_target_properties(
        oatpp
        oatpp-test
        cpp_redis
        tacopie
        PROPERTIES FOLDER libs
    )

    add_definitions(/wd4244 /bigobj)
    # https://stackoverflow.com/a/54531576
    file(GLOB_RECURSE _source_list *.c* *.cpp* *.h* *.hpp*)
    foreach(_source IN ITEMS ${_source_list})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_group_path}")
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})
endif()