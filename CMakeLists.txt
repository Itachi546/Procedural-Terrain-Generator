cmake_minimum_required(VERSION 3.5.0)
project(TerrainGenerator)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE PROJECT_HEADER_FILES CONFIGURE_DEPENDS Include/*.h)
file(GLOB_RECURSE PROJECT_SOURCE_FILES CONFIGURE_DEPENDS Source/*.cpp)

list(APPEND PROJECT_SOURCE_FILES External/glad/src/glad.c)
list(APPEND PROJECT_HEADER_FILES External/stb_image/include/stb_image.h)

add_subdirectory(External/glm)
add_subdirectory(External/glfw)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE 
Include/
External/glm
External/glfw/include
External/glad/include
External/stb_image/include
)

target_link_libraries(${PROJECT_NAME} glfw glm)

target_compile_definitions(${PROJECT_NAME} PUBLIC
GLM_ENABLE_EXPERIMENTAL
)