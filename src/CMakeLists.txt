add_subdirectory (http)
add_subdirectory (utility)
add_subdirectory (nano)

add_executable (
    http_test
    main.cpp
)

target_include_directories(
    http_test
    PRIVATE
        $ENV{PROJECT_SRC_DIR}
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries (http_test
    PRIVATE
    http
    utility
    pthread
)

install (
    TARGETS http_test
    DESTINATION $ENV{PROJECT_BIN_DIR}/
)
