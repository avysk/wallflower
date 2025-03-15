cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(wallflower)

set(PLUGIN "wallflower")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS DBus Gui Network Widgets REQUIRED)
find_package(lxqt 2.1.0 REQUIRED)

add_library(${PLUGIN} MODULE
    wallflower.h wallflower.cpp
    basesearcher.h basesearcher.cpp
    downloader.h downloader.cpp
    picture.h
    podibasu.h podibasu.cpp
    searcher.h searcher.cpp
    searcherconcepts.h
    searcherinterface.h
    sites/wallheavensearcher.h sites/wallheavensearcher.cpp)
target_link_libraries(${PLUGIN} Qt6::DBus Qt6::Gui Qt6::Network Qt6::Widgets lxqt)

set(INSTALL_DIR "$ENV{HOME}/.local")
install(TARGETS ${PLUGIN}
    DESTINATION "${INSTALL_DIR}/lib/lxqt-panel")
install(FILES ${PLUGIN}.desktop
    DESTINATION "${INSTALL_DIR}/share/lxqt/lxqt-panel")
