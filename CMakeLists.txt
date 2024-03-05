cmake_minimum_required(VERSION 3.11)
project(OpenGLTempl)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCE_FILES "src/*")

# Including GLFW
add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

# Include GLM
add_subdirectory(lib/glm)
include_directories(lib/glm/glm)

# Add glad
include_directories(lib/glad/include)
list(APPEND SOURCE_FILES "lib/glad/src/glad.c")

# Add IMGUI
include_directories(lib/imgui)
file(GLOB LIB_FILES "lib/imgui/*.cpp" "lib/imgui/backends/imgui_impl_opengl3.cpp" "lib/imgui/backends/imgui_impl_glfw.cpp")
list(APPEND SOURCE_FILES ${LIB_FILES})

# Include STB (My Image Loader) Need to switch to FreeImage
include_directories(lib/stb)



add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

# Linking GLFW, GLM and OpenGL
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glfw glm ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})
