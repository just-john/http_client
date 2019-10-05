
set (SOURCE_CODE
    nano_requests.cpp
)

add_library (nanoverse STATIC ${SOURCE_CODE})

target_include_directories (
    nanoverse
    PUBLIC
        ${Boost_INCLUDE_DIR}

    PRIVATE
        $ENV{PROJECT_SRC_DIR}
)

target_link_libraries (
    nanoverse
)

install ( TARGETS nanoverse
    DESTINATION $ENV{PROJECT_BIN_DIR}/lib/ )

file(GLOB headers
    $ENV{PROJECT_SRC_DIR}/nano/api/request/*.hpp)

install (FILES ${headers}
    DESTINATION $ENV{PROJECT_BIN_DIR}/include/nano/api/request/)