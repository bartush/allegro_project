cmake_minimum_required(VERSION 3.5)

project(allegro_project)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR})
#AUX_SOURCE_DIRECTORY(dir $ENV{IMGUI_FOLDER})
set(SOURCES allegro_project.cpp test.cpp
    $ENV{IMGUI_FOLDER}/backends/imgui_impl_allegro5.cpp
    $ENV{IMGUI_FOLDER}/imgui.cpp
    $ENV{IMGUI_FOLDER}/imgui_draw.cpp
    $ENV{IMGUI_FOLDER}/imgui_demo.cpp
    $ENV{IMGUI_FOLDER}/imgui_tables.cpp
    $ENV{IMGUI_FOLDER}/imgui_widgets.cpp)

#set(ALLEGRO_PROJECT_LIBS -lGL -lGLU -lallegro /usr/lib/x86_64-linux-gnu/liballegro_font.so /usr/lib/x86_64-linux-gnu/liballegro_ttf.so /usr/lib/x86_64-linux-gnu/liballegro_primitives.so /usr/lib/x86_64-linux-gnu/liballegro_color.so /usr/lib/x86_64-linux-gnu/liballegro_image.so)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -mwindows -O0 -g")
#set(ALLEGRO_PROJECT_LIBS -lopengl32 -lglu32 -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_color -lallegro_image)
set(ALLEGRO_PROJECT_LIBS -lopengl32 -lglu32 -lallegro_monolith)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME}
 PUBLIC
 $ENV{IMGUI_FOLDER}
 $ENV{IMGUI_FOLDER}/backends)
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_PROJECT_LIBS})
add_compile_definitions(ALLEGRO_PROJECT_OPENGL)
add_compile_definitions(IMGUI_USER_CONFIG=\"$ENV{IMGUI_FOLDER}/examples/example_allegro5/imconfig_allegro5.h\")

#add_custom_command(
#  TARGET ${PROJECT_NAME} POST_BUILD
#  COMMAND allegro_project.exe
#  VERBATIM)


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#-lGL -lGLU -lallegro /usr/lib/x86_64-linux-gnu/liballegro_font.so /usr/lib/x86_64-linux-gnu/liballegro_primitives.so /usr/lib/x86_64-linux-gnu/liballegro_color.so /usr/lib/x86_64-linux-gnu/liballegro_image.so


#cmake_minimum_required(VERSION 2.8)
#project(cmake-example)
#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED ON)
#find_package(Boost COMPONENTS chrono filesystem REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
#set(SRC_LIST lib1/main.cpp lib2/main.cpp main.cpp)
#add_executable(${PROJECT_NAME} ${SRC_LIST})
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
