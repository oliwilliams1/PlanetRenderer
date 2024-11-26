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

# Use g++ if not MSVC, if not MSVC use manual IMGUI installation
if(NOT MSVC)
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
    target_sources(PlanetRenderer PRIVATE
        "imgui_impl_glfw.cpp"
        "imgui_impl_opengl3.cpp"
    )
endif()

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Add source to this project's executable.
add_executable(PlanetRenderer "main.cpp" "PlanetRenderer.h" "object.cpp" "object.h" "shader.h" "shader.cpp" "camera.h" "camera.cpp")

# Add imgui files if user is not using VCPKG
if (NOT MSVC)
    log("Using imgui from source")
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

# Add shader folder to build output
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

file(GLOB SHADERS "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag")

add_custom_command(TARGET PlanetRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SHADER_DIR} $<TARGET_FILE_DIR:PlanetRenderer>/shaders)