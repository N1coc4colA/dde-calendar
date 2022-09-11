project(python_calendar)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

add_definitions(-DQT_NO_KEYWORDS)

find_package(Qt5Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(PythonLibs 3.10 REQUIRED)

add_library(python_calendar SHARED ./src/python_plugin.h ./src/python_plugin.cpp ./src/bindings.cpp ./src/bindings.h)

target_link_libraries(python_calendar PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    ${PYTHON_LIBRARIES}
)

target_include_directories(python_calendar PUBLIC /usr/include/python3.10)

target_include_directories(python_calendar PUBLIC
    ../../src
)

install(TARGETS python_calendar DESTINATION /usr/lib/dde-calendar)
