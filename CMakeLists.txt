cmake_minimum_required(VERSION 3.16)

project(LoopSolidNative LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenVDB CONFIG QUIET)
if (OpenVDB_FOUND)
    set(OPENVDB_TARGET OpenVDB::openvdb)
else()
    find_path(OPENVDB_INCLUDE_DIR
        NAMES openvdb/openvdb.h
    )
    find_library(OPENVDB_LIBRARY
        NAMES openvdb
    )

    if (NOT OPENVDB_INCLUDE_DIR OR NOT OPENVDB_LIBRARY)
        message(FATAL_ERROR "OpenVDB not found")
    endif()

    add_library(OpenVDB::openvdb UNKNOWN IMPORTED)
    set_target_properties(OpenVDB::openvdb PROPERTIES
        IMPORTED_LOCATION "${OPENVDB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENVDB_INCLUDE_DIR}"
    )
    set(OPENVDB_TARGET OpenVDB::openvdb)
endif()

add_library(loopsolidnative SHARED
    cpp/openvdb/offset_mesh.cpp
    cpp/loopsolid_native.cpp
)

target_link_libraries(loopsolidnative
    PRIVATE
        ${OPENVDB_TARGET}
)

target_include_directories(loopsolidnative
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp
)
