cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(wallflower)

set(PLUGIN "wallflower")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(REQUIRED_QT_VERSION "6.6.0")
set(LXQT_GLOBALKEYS_MINIMUM_VERSION "2.1.0")
set(LXQT_MINIMUM_VERSION "2.1.0")

find_package(Qt6Widgets ${REQUIRED_QT_VERSION} REQUIRED)
find_package(lxqt ${LXQT_MINIMUM_VERSION} REQUIRED)

include(LXQtPreventInSourceBuilds)

include(LXQtCompilerSettings NO_POLICY_SCOPE)

set(HEADERS
  "${PLUGIN}plugin.h"
)

set(SOURCES
  "${PLUGIN}plugin.cpp"
)

add_library(${PLUGIN} MODULE ${HEADERS} ${SOURCES})
target_link_libraries(${PLUGIN} Qt6::Widgets lxqt)
install(TARGETS ${PLUGIN} DESTINATION $ENV{HOME}/.local/lib/lxqt-panel)
install(FILES ${PLUGIN}.desktop
    DESTINATION $ENV{HOME}/.local/share/lxqt/lxqt-panel)
