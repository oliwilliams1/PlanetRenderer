cmake_minimum_required(VERSION 3.12)

project("PlanetRenderer" CXX)

# Set the project name and compilers
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

# Find required packages
find_package(OpenGL REQUIRED)  # Add OpenGL package
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

# Add imgui files if user is not using VCPKG
if (NOT MSVC)
    target_sources(PlanetRenderer PRIVATE
        "imgui_impl_glfw.cpp"
        "imgui_impl_opengl3.cpp"
    )
endif()

# Link libraries
target_link_libraries(PlanetRenderer PRIVATE 
    glfw 
    glm::glm 
    GLEW::GLEW 
    OpenGL::GL
    imgui::imgui
)

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