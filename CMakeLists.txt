cmake_minimum_required (VERSION 2.8)

project(tinyxr_win_sdk_demo)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libs)
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/libs)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libs/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/libs/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)

aux_source_directory(./src DIR_SRCS)

add_executable(GojoyHeadTrackerDemo ${DIR_SRCS})

target_link_libraries(GojoyHeadTrackerDemo freeglut glew32 glfw3 hidapi)