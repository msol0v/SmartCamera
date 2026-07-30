cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME SmartCamera)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Core/Src/FreeRTOS_CLI.c
        Core/Src/cli.c
        Core/Inc/motor_task.h
        Core/Src/motor_task.c
        Core/Src/pwm_task.c
        Core/Inc/pwm_task.h
        Core/Inc/router.h
        Core/Src/router.c
        Core/Inc/api.h
        Core/Src/api.c
        Core/Src/cmd.c
        Core/Inc/stepper.h
        Core/Src/stepper.c
        Core/Src/autotest_task.c
        Core/Inc/w25q_mem.h
        Core/Src/w25q_mem.c
        Core/Inc/lfs_util.h
        Core/Src/lfs_util.c
        Core/Inc/lfs.h
        Core/Src/lfs.c
        Core/Inc/lfs_w25q_adapter.h
        Core/Src/lfs_w25q_adapter.c
        Core/Inc/app_storage.h
        Core/Src/app_storage.c
        Core/Inc/cert.h)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
        LFS_THREADSAFE
        #LFS_YES_TRACE // Если надо отладить ФС
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
