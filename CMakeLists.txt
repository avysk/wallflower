cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX /usr/local CACHE PATH "PREFIX=/usr/local" FORCE)
endif()

project(lxqt-panel)

set(PLUGIN "wallflower")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(REQUIRED_QT_VERSION "6.6.0")
set(KF6_MINIMUM_VERSION "6.0.0")
set(LXQT_GLOBALKEYS_MINIMUM_VERSION "2.1.0")
set(LXQT_MINIMUM_VERSION "2.1.0")

find_package(Qt6Widgets ${REQUIRED_QT_VERSION} REQUIRED)
find_package(lxqt ${LXQT_MINIMUM_VERSION} REQUIRED)

set(LXQT_PANEL_PATCH_VERSION 1)
set(LXQT_PANEL_VERSION ${LXQT_MAJOR_VERSION}.${LXQT_MINOR_VERSION}.${LXQT_PANEL_PATCH_VERSION})
add_definitions("-DLXQT_PANEL_VERSION=\"${LXQT_PANEL_VERSION}\"")

include(LXQtPreventInSourceBuilds)
include(LXQtTranslate)

message(STATUS "==OLD== CMAKE_MODULE_LINKER_FLAGS: ${CMAKE_MODULE_LINKER_FLAGS}")
set( OLD_CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
include(LXQtCompilerSettings NO_POLICY_SCOPE)
set(CMAKE_MODULE_LINKER_FLAGS "${OLD_CMAKE_MODULE_LINKER_FLAGS} ${SYMBOLIC_FLAGS}")

set(PLUGIN_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/lxqt-panel")
add_definitions(
    -DPLUGIN_DIR=\"${PLUGIN_DIR}\"
)

message(STATUS "CMAKE Module linker flags: ${CMAKE_MODULE_LINKER_FLAGS}")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

#########################################################################

include_directories("/usr/include/lxqt/")
set(HEADERS
  "${PLUGIN}plugin.h"
)

set(SOURCES
  "${PLUGIN}plugin.cpp"
)

include ("cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})

install(FILES wallflower.desktop DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/lxqt/lxqt-panel)
