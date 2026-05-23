# This file is part of the IEX2H5 project and is licensed under the MIT License.
# 
# Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada 🇨🇦
# Contact: info@vargaconsulting.ca 

set(name h5cpp)

set(libname lib${name})
project(${libname} LANGUAGES CXX)

set(current_version_dir "v1.12.4")
add_library(${libname} INTERFACE)
target_include_directories(${libname} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${current_version_dir}/include)

if(H5CPP_LIBDEFLATE_FOUND)
    target_link_libraries(${libname} INTERFACE ${H5CPP_LIBDEFLATE_TARGET})
    target_compile_definitions(${libname} INTERFACE H5CPP_HAS_LIBDEFLATE=1)
else()
    target_compile_definitions(${libname} INTERFACE H5CPP_DISABLE_LIBDEFLATE=1)
endif()

if(H5CPP_USE_ZSTD AND H5CPP_ZSTD_FOUND)
    target_link_libraries(${libname} INTERFACE ${H5CPP_ZSTD_TARGET})
endif()
