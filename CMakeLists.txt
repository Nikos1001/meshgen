
project(lbp)

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

add_executable(lbp 
    src/util.c
    src/geoutil.c src/polygon.c src/material.c
    src/renderer.c src/shader.c src/mesh.c src/texture.c src/framebuffer.c
    src/main.c
)

target_link_libraries(
    lbp
    glfw
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    cglm_headers
)

add_subdirectory(lib/cglm/ EXCLUDE_FROM_ALL)