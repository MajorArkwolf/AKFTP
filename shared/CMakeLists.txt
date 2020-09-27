# Set cmake minimum version.
cmake_minimum_required(VERSION 3.14)

# Set the project details.
set(PROJECT_NAME myftp-shared)
project(${PROJECT_NAME} LANGUAGES C)

# Clang sanitizer settings.
set(SANITIZER_OS "Darwin,Linux")
set(SANITIZER_FLAGS "-fsanitize=address,undefined,leak")

# Treat warnings as errors.
option(WarningsAsErrors "WarningsAsErrors" OFF)

# Disable in-source builds.
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

# Set build type to debug by default.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add Linux flag.
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()


# Set build output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG    ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${PROJECT_BINARY_DIR})

add_library(${PROJECT_NAME} SHARED
        src/Token.c
        src/Message.c)

# Define the executable.
# add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)



