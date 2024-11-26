cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

# Set the project name
project("PlanetRenderer")

# Detect the compiler and set the toolchain file if using MSVC
if (MSVC)
    set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

    # Enable Hot Reload for MSVC compilers if supported.
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()
else()
    # For non-MSVC compilers, you can set additional configurations if needed
    message(STATUS "Using non-MSVC compiler")
endif()

# Find required packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Add source to this project's executable.
add_executable(PlanetRenderer
    "main.cpp"
    "PlanetRenderer.h"
    "object.cpp"
    "object.h"
    "shader.h"
    "shader.cpp"
    "camera.h"
    "camera.cpp"
)

# Link libraries
target_link_libraries(PlanetRenderer PRIVATE glfw)
target_link_libraries(PlanetRenderer PRIVATE glm::glm)
target_link_libraries(PlanetRenderer PRIVATE GLEW::GLEW)
target_link_libraries(PlanetRenderer PRIVATE imgui::imgui)

# Set C++ standard
set_property(TARGET PlanetRenderer PROPERTY CXX_STANDARD 20)

# Add shaders to build
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# Collect shader files
file(GLOB SHADERS "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag")

# Copy shaders to the output directory after the build
add_custom_command(TARGET PlanetRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SHADER_DIR} $<TARGET_FILE_DIR:PlanetRenderer>/shaders)