cmake_minimum_required(VERSION 2.8)

project(allegro_project)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/bin)
set(SOURCES allegro_project.cpp test.cpp)
set(ALLEGRO_PROJECT_LIBS -lGL -lGLU -lallegro /usr/lib/x86_64-linux-gnu/liballegro_font.so /usr/lib/x86_64-linux-gnu/liballegro_ttf.so /usr/lib/x86_64-linux-gnu/liballegro_primitives.so /usr/lib/x86_64-linux-gnu/liballegro_color.so /usr/lib/x86_64-linux-gnu/liballegro_image.so)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_PROJECT_LIBS})

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
