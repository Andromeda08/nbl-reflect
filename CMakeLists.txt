cmake_minimum_required(VERSION 3.30)
project(nblReflect)

set(CMAKE_CXX_STANDARD 23)

set(nblReflectTestTarget OFF)

find_package(Vulkan REQUIRED)
set(SPV_REFLECT "ext/spv-reflect/spirv_reflect.h" "ext/spv-reflect/spirv_reflect.cpp")

add_library(nblReflect
    ${SPV_REFLECT}
    include/nbl/reflect/ShaderReflection.hpp
    src/ShaderReflection.cpp
    src/ShaderReflectionUtils.cpp
)

target_include_directories(nblReflect PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    ./include/nbl
    ./ext/spv-reflect
)

if (nblReflectTestTarget)
    add_executable(nblReflectTest test/test.cpp)
    target_include_directories(nblReflectTest PRIVATE ./include/nbl)
    target_link_libraries(nblReflectTest PRIVATE nblReflect)
endif()