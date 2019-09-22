
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
    LIBRARY
        DESTINATION $ENV{PROJECT_BIN_DIR}

    PUBLIC_HEADER
        DESTINATION $ENV{PROJECT_BIN_DIR}/include
)