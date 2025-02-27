cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(wallflower)

set(PLUGIN "wallflower")

set(CMAKE_BUILD_TYPE Release)

find_package(Qt6Widgets 6.8.0 REQUIRED)
find_package(lxqt 2.1.0 REQUIRED)

add_library(${PLUGIN} MODULE wallflowerplugin.cpp wallflowerplugin.h)
target_link_libraries(${PLUGIN} Qt6::Widgets lxqt)

set(INSTALL_DIR "$ENV{HOME}/.local")
install(TARGETS ${PLUGIN}
    DESTINATION "${INSTALL_DIR}/lib/lxqt-panel")
install(FILES ${PLUGIN}.desktop
    DESTINATION "${INSTALL_DIR}/share/lxqt/lxqt-panel")
