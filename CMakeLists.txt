cmake_minimum_required(VERSION 3.10)
project(WD_Tetrahedralization)

find_package(CGAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(include extern/imgui extern/imgui/backends)

add_executable(viewer 
    src/viewer.cpp
    extern/imgui/imgui.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/backends/imgui_impl_glfw.cpp
    extern/imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(viewer 
    CGAL::CGAL 
    OpenGL::GL 
    OpenGL::GLU 
    GLEW::GLEW 
    glfw
)
