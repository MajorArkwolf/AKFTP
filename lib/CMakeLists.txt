# Build libraries.
string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Disable compiler warnings.
add_compile_options(
    # Clang
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-everything>
    # GCC
    $<$<CXX_COMPILER_ID:GNU>:-w>
    # Visual Studio
    $<$<CXX_COMPILER_ID:MSVC>:/W0>
)
add_subdirectory(json-c)
add_subdirectory(base64)

# Ignore warnings from library headers.
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    json-c/
    base64/
)
