cmake_minimum_required (VERSION 3.12)

# Set toolchain file for VCPKG if on windows
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PlanetRenderer" CXX)

# Find packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)

# Add imgui include dir for bindings
include_directories(${CMAKE_SOURCE_DIR}/imgui)
include_directories(${Stb_INCLUDE_DIR})

# Add source
set(SOURCE_FILES
    "src/main.cpp"
    "src/object.cpp"
    "src/shader.cpp"
    "src/camera.cpp"
    "src/planet.cpp"
    "src/noise.cpp"
    "src/utils.cpp"
    "src/app.cpp"
    "src/deferredRenderer.cpp"
)

# Add header files
set(HEADER_FILES
    "src/object.h"
    "src/shader.h"
    "src/camera.h"
    "src/planet.h"
    "src/noise.h"
    "src/utils.h"
    "src/app.h"
    "src/deferredRenderer.h"
)

# Add executable
add_executable(PlanetRenderer ${SOURCE_FILES} ${HEADER_FILES} "src/treesHandler.h" "src/treesHandler.cpp")

# Add imgui source files if user is not using VCPKG
if (NOT MSVC)
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
    target_sources(PlanetRenderer PRIVATE
        "imgui/imgui_impl_glfw.cpp"
        "imgui/imgui_impl_opengl3.cpp"
    )
endif()

# Link libraries
target_link_libraries(PlanetRenderer PRIVATE 
    glfw 
    glm::glm 
    GLEW::GLEW
    imgui::imgui
)

# Link OpenGL if not on windows
if(NOT WIN32)
    find_package(OpenGL REQUIRED)
    target_link_libraries(PlanetRenderer PRIVATE OpenGL::GL)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PlanetRenderer PROPERTY CXX_STANDARD 20)
endif()

# Gather shaders for copying
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
file(GLOB SHADERS "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.tesc" "${SHADER_DIR}/*.tese" "${SHADER_DIR}/*.comp")

# Custom command to copy shaders to the build output
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADERS}
        ${CMAKE_CURRENT_BINARY_DIR}/shaders/
    DEPENDS ${SHADERS}
    COMMENT "Copying shaders to build output"
)

# Ensure shaders are copied
add_custom_target(copy_shaders ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_dependencies(PlanetRenderer copy_shaders)