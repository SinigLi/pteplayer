cmake_minimum_required(VERSION 3.16)
project(pteutil VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)

qt_standard_project_setup()

qt_add_library(pteutil STATIC
    date.h
    enumflags.h
    enumtostring.h
    enumtostring_fwd.h
    scopeexit.h
    settingstree.cpp settingstree.h
    tostring.h
    toutf8.h
    version.cpp version.h
)

target_include_directories(pteutil PUBLIC
    ..
    ../third_part
    ${BOOST_INCLUDE_PATH}
)

target_compile_definitions(pteutil PUBLIC
    PTE_VERSION=2.0.0-alpha19
)

target_link_libraries(pteutil PUBLIC
    Qt::Core
    Qt::Gui
)

install(TARGETS pteutil
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
