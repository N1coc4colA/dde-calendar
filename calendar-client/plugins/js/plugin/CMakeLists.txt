project(js_calendar)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

find_package(Qt5Widgets REQUIRED)
find_package(PkgConfig REQUIRED)

#Node Js does not provide any PkgConfig or any CMake configs.
add_library(NodeJs SHARED IMPORTED)
set_target_properties(NodeJs PROPERTIES IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libnode.so)


file (GLOB HEADERS "./src/*.h" "./src/bindings/*.h")
file (GLOB SOURCES "./src/*.cpp" "./src/bindings/*.cpp")

QT5_WRAP_CPP(tst_hdr_moc ${HEADERS})

set_property(SOURCE ./src/bindings/bindings.h PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ./src/bindings/bindings.cpp PROPERTY SKIP_AUTOMOC ON)
add_library(js_calendar SHARED ${tst_hdr_moc} ${SOURCES})

set_property(SOURCE ./src/bindings/bindings.h PROPERTY SKIP_AUTOMOC ON)

target_link_libraries(js_calendar PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    NodeJs
    v8pp #The v8/C++ library wrapper
)

target_include_directories(js_calendar PUBLIC /usr/include/node)
target_include_directories(js_calendar PUBLIC
    ../../../src
    ../v8pp
)

install(TARGETS js_calendar DESTINATION /usr/lib/dde-calendar)
