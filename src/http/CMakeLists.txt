
set (SOURCE_CODE
    client.cpp
    response.cpp
    request.cpp
)

add_library (http STATIC ${SOURCE_CODE})

target_include_directories (
    http
    PUBLIC
        ${Boost_INCLUDE_DIR}

    PRIVATE
        $ENV{PROJECT_SRC_DIR}

)

target_link_libraries (
    http
    PUBLIC
        ssl
        crypto

    PRIVATE
        utility
        nanoverse
)

install (
    TARGETS http
    LIBRARY
        DESTINATION $ENV{PROJECT_BIN_DIR}/lib/
)

file (
    GLOB headers
    $ENV{PROJECT_SRC_DIR}/http/*.hpp
)

install (
    FILES ${headers}
    DESTINATION $ENV{PROJECT_BIN_DIR}/include/http/
)