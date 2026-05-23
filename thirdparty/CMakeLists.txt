# This file is part of the IEX2H5 project and is licensed under the MIT License.
# 
# Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada 🇨🇦
# Contact: info@vargaconsulting.ca 

add_subdirectory(argparse)
add_subdirectory(armadillo)
add_subdirectory(date)
add_subdirectory(fmt)
add_subdirectory(doctest)
option(H5CPP_USE_LIBDEFLATE "Use libdeflate for H5CPP deflate/gzip filters when available" ON)
option(H5CPP_REQUIRE_LIBDEFLATE "Fail configuration if H5CPP_USE_LIBDEFLATE is ON and libdeflate is not found" OFF)
option(H5CPP_USE_ZSTD "Use Zstd compression filter when available" ON)
add_subdirectory(libdeflate)
add_subdirectory(zstd)
add_subdirectory(h5cpp)
add_subdirectory(hiredis)
add_subdirectory(rediscpp)
add_subdirectory(zlibng)
add_subdirectory(hdf5)
