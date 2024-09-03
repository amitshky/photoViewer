file(GLOB_RECURSE SRC_FILES "*.cpp")

add_executable(
    ${PROJECT_NAME}
    "${SRC_FILES}"
    # "../ext/TinyEXIF/TinyEXIF.cpp"
)
