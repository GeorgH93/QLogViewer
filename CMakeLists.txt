cmake_minimum_required(VERSION 3.15.0)

project(QLogViewer VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
  set(CMAKE_WIN32_EXECUTABLE ON)
endif()

find_package(Qt6 COMPONENTS Widgets REQUIRED)


file(GLOB SRC_FILES "src/*.cpp" "src/*.ui")
add_executable(QLogViewer "src/main.cpp")

target_sources(QLogViewer PRIVATE ${SRC_FILES})

include_directories(src)


target_link_libraries(QLogViewer Qt6::Widgets)


include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master
)

set(YAML_CPP_BUILD_TESTS OFF)
FetchContent_MakeAvailable(yaml-cpp)
target_link_libraries(QLogViewer yaml-cpp)
