cmake_minimum_required(VERSION 3.1)
project(iscore_addon_segments LANGUAGES CXX)

iscore_common_setup()

# Files & main target
set(HDRS
"${CMAKE_CURRENT_SOURCE_DIR}/Segments/Visitor.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/iscore_addon_segments.hpp"
)
set(SRCS
"${CMAKE_CURRENT_SOURCE_DIR}/iscore_addon_segments.cpp"
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})
target_link_libraries(${PROJECT_NAME} PUBLIC
    iscore_lib_base iscore_lib_process
    iscore_plugin_scenario iscore_plugin_automation
    iscore_plugin_loop iscore_addon_image)

if(TARGET iscore_plugin_audio)
    target_link_libraries(${PROJECT_NAME} PUBLIC iscore_plugin_audio)
endif()

setup_iscore_plugin(${PROJECT_NAME})
