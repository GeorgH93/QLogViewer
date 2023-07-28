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
# Remove main.cpp from the shared library sources
list(FILTER SRC_FILES EXCLUDE REGEX ".*main\.cpp$")

# Shared library of all sources (used by both app and tests)
add_library(QLogViewerLib STATIC ${SRC_FILES})
target_include_directories(QLogViewerLib PUBLIC src)
target_link_libraries(QLogViewerLib Qt6::Widgets)

add_executable(QLogViewer "src/main.cpp")
target_include_directories(QLogViewer PRIVATE src)
target_link_libraries(QLogViewer QLogViewerLib)


include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master
)

set(YAML_CPP_BUILD_TESTS OFF)
FetchContent_MakeAvailable(yaml-cpp)
target_link_libraries(QLogViewerLib yaml-cpp)

# ---- Tests ----
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

file(GLOB TEST_SOURCES "tests/*.cpp")
add_executable(QLogViewerTests ${TEST_SOURCES})
target_include_directories(QLogViewerTests PRIVATE src)
target_link_libraries(QLogViewerTests PRIVATE
    QLogViewerLib
    Catch2::Catch2
)

include(CTest)
include(Catch)
catch_discover_tests(QLogViewerTests)
