
set (SOURCE_CODE
    string_functions.cpp
)

add_library (utility STATIC ${SOURCE_CODE})

target_include_directories (
    utility
    PRIVATE $ENV{PROJECT_SRC_DIR}
)

install (
    TARGETS utility
    DESTINATION $ENV{PROJECT_BIN_DIR}/lib/
)

file(GLOB headers
    $ENV{PROJECT_SRC_DIR}/utility/*.hpp)

install (FILES ${headers}
    DESTINATION $ENV{PROJECT_BIN_DIR}/include/utility/)