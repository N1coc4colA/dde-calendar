project(python_calendar)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

find_package(Qt5Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(PythonLibs 3.9 REQUIRED)

add_executable(python_calendar_bin ./src/main.cpp)
add_library(python_calendar_lib SHARED ./src/python_plugin.h ./src/python_plugin.cpp ./src/bindings.cpp ./src/bindings.h)

target_link_libraries(python_calendar_bin PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    ${PYTHON_LIBRARIES}
)
target_link_libraries(python_calendar_lib PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    ${PYTHON_LIBRARIES}
)

target_include_directories(python_calendar_bin PUBLIC /usr/include/python3.9)
target_include_directories(python_calendar_lib PUBLIC /usr/include/python3.9)

target_include_directories(python_calendar_bin PUBLIC
    ../../src
)
target_include_directories(python_calendar_lib PUBLIC
    ../../src
)
