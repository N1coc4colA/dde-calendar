project(calendar_sample)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets REQUIRED)

add_library(calendar_sample SHARED sample.cpp sample.h)

target_link_libraries(calendar_sample PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

target_include_directories(calendar_sample PUBLIC
    ../../src
)
