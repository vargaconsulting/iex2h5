# This file is part of the IEX2H5 project and is licensed under the MIT License.
# 
# Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada 🇨🇦
# Contact: info@vargaconsulting.ca 


cmake_minimum_required(VERSION 3.22.0)

# Set the project name and specify the required C++ standard
project(iex2h5 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(WARNING "In-source builds are discouraged. Skipping compile_commands.json symlink.")
else()
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
    RESULT_VARIABLE result
    OUTPUT_QUIET ERROR_QUIET
  )
endif()

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

option(USE_GOOGLE_PROFILER "Enable Google CPU profiler if found" OFF)
option(IEX2H5_BUILD_PARQUET "Enable Apache Parquet output support (requires Arrow)" OFF)

# automated versioning
execute_process( COMMAND git rev-parse --short HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process( COMMAND git describe --tags --abbrev=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE GIT_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)

# Optionally, use the tag and hash to create a version string.
# If no tag is available, you can fallback to just using the hash.
# Configure the version file with the Git information
if(GIT_TAG)
  set(IEX2H5_VERSION_STRING ${GIT_TAG})
else()
  set(IEX2H5_VERSION_STRING ${GIT_COMMIT_HASH})
endif()
# Specify IEX2H5 system related constants, these value also can be (re)configured through env variables:
add_definitions(
  -DIEX2H5_SOFTWARE_VERSION="${IEX2H5_VERSION_STRING}"
  -DIEX2H5_SOFTWARE_COMMIT_HASH="${GIT_COMMIT_HASH}"
  -DARMA_DONT_USE_WRAPPER
  -DHOST_NAME_MAX=254
  -DVALUE_PRECISION=6
  -DSTAT_PRECISION=4
  -DIEX_MAX_SYMBOLS=65536 )

## begin thirdparty library dependencies
  add_subdirectory(thirdparty EXCLUDE_FROM_ALL)
## end library dependencies

include(FindPackageHandleStandardArgs)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED QUIET)
find_path(GOOGLE_PROFILER_INCLUDE_DIR NAMES google/profiler.h)
find_library(GOOGLE_PROFILER_LIBRARY NAMES profiler)
find_package_handle_standard_args(GoogleProfiler DEFAULT_MSG GOOGLE_PROFILER_INCLUDE_DIR GOOGLE_PROFILER_LIBRARY)
find_package(Python3 REQUIRED)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 11.4.0)
    add_compile_options(-w)
endif()

list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include ${HDF5_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} )
list(APPEND LIBS 
  ${HDF5_C_LIBRARIES} ${ZLIB_LIBRARIES} 
  libargparse libarmadillo libh5cpp libdate
  libfmt_static libhiredis_static librediscpp_static libzng_static libhdf5_static
  )

if(USE_GOOGLE_PROFILER AND GoogleProfiler_FOUND)
  message(STATUS "Google Profiler enabled")
  list(APPEND LIBS ${GOOGLE_PROFILER_LIBRARY})
  add_definitions(-DHAVE_GOOGLE_PROFILER)
endif()

if(IEX2H5_BUILD_PARQUET)
  find_package(Arrow REQUIRED)
  find_package(Parquet REQUIRED)
  message(STATUS "Apache Parquet support enabled")
  list(APPEND LIBS Arrow::arrow_shared Parquet::parquet_shared)
  add_definitions(-DIEX2H5_HAVE_PARQUET)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(IEX_PCAP_FILE "${CMAKE_SOURCE_DIR}/data/iex-packets.pcap")
  set(CMAKE_CXX_FLAGS "-fsanitize=address,undefined,bounds,float-divide-by-zero,float-cast-overflow,alignment -Wall -Wextra -Wpedantic -Werror")
  add_definitions(-DDEBUG) # -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC !!! will break AWS !!!
  enable_testing()
  ## usage: `add_test_case(file_name [LIBRARIES library-01 library-02 .. library-0n])`
  function(add_test_case file_name)
      set(options)
      set(one_value_args)
      set(multi_value_args LIBRARIES)
      cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

      set(test_name test-${file_name})
      add_executable(${test_name} test/${file_name}.cpp)
      set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
      target_include_directories(${test_name} PRIVATE ${INCLUDE_DIRS} src)
      target_compile_options(${test_name} PRIVATE -w)
      target_compile_definitions(${test_name} PRIVATE IEX_PCAP_FILE="${IEX_PCAP_FILE}")

      # Link libraries, excluding INTERFACE libraries
      target_link_libraries(${test_name} PRIVATE ${LIBS} libdoctest)
      foreach(lib IN LISTS ARG_LIBRARIES)
        target_link_libraries(${test_name} PRIVATE ${lib})
      endforeach()
      add_test(NAME ${test_name} COMMAND ${test_name})
  endfunction()

  add_test_case(example)
  add_test_case(utils)
  add_test_case(filters)
  add_test_case(error)
  add_test_case(generics)
endif()

## APPLICATION
# License managenment
set(LICENSES_INPUT_DIR ${CMAKE_SOURCE_DIR}/docs/licenses)
set(LICENSES_OUTPUT_FILE ${CMAKE_BINARY_DIR}/include/third_party_licenses.hpp)
set(LICENSES_PARSER ${CMAKE_SOURCE_DIR}/scripts/licence-parser.py)

add_custom_command(
    OUTPUT ${LICENSES_OUTPUT_FILE}  COMMAND ${Python3_EXECUTABLE} ${LICENSES_PARSER} DEPENDS ${LICENSES_PARSER}
    COMMENT "Generating third_party_licenses.hpp from license texts"
    VERBATIM
)

file(GLOB_RECURSE all_source_files CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE all_header_files CONFIGURE_DEPENDS src/*.hpp)

add_executable(${PROJECT_NAME} ${all_source_files})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_compile_options(${PROJECT_NAME} PRIVATE -w -fno-omit-frame-pointer)
target_compile_definitions(${PROJECT_NAME} PRIVATE ARMA_NO_DEBUG IEX_PROGRAM_NAME="${PROJECT_NAME}")
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS} src)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>
        COMMENT "Stripping symbols from ${PROJECT_NAME} binary"
    )
endif()

add_custom_target(generate_licenses DEPENDS ${LICENSES_OUTPUT_FILE})
add_dependencies(${PROJECT_NAME} generate_licenses)
