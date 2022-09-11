project(google_calendar)

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
find_package(PythonLibs 3.10 REQUIRED)

add_library(google_calendar SHARED ./src/bindings/bindings.h ./src/bindings/bindings.cpp ./src/plugin.cpp ./src/python/pybinds.qrc)

target_link_libraries(google_calendar PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    ${PYTHON_LIBRARIES}
)

target_include_directories(google_calendar PUBLIC /usr/include/python3.9)

target_include_directories(google_calendar PUBLIC
    ../../src
)

install(TARGETS google_calendar DESTINATION /usr/lib/dde-calendar)
