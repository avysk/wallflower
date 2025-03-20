cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

project(wallflower VERSION 1.4.1)

enable_testing()

set(PLUGIN "wallflower")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_AUTOMOC ON)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN}.desktop.in
    ${CMAKE_BINARY_DIR}/${PLUGIN}.desktop
)
include_directories(${CMAKE_BINARY_DIR})

find_package(Qt6 COMPONENTS Core DBus Gui Network Widgets REQUIRED)
find_package(lxqt 2.1.0 REQUIRED)

set(UI_FILES wallflowersettings.ui)
qt6_wrap_ui(GENERATED_UI_FILES ${UI_FILES})

add_library(${PLUGIN} MODULE
    wallflower.h wallflower.cpp
    basesearcher.h basesearcher.cpp
    downloader.h downloader.cpp
    picture.h
    podibasu.h podibasu.cpp
    searcher.h searcher.cpp
    searcherconcepts.h
    searcherinterface.h
    sites/wallheavensearcher.h sites/wallheavensearcher.cpp
    version.h
    wallflowersettings.h wallflowersettings.cpp
    ${GENERATED_UI_FILES})
target_link_libraries(
    ${PLUGIN} Qt6::DBus Qt6::Gui Qt6::Network Qt6::Widgets lxqt
)

set(INSTALL_DIR "$ENV{HOME}/.local")
install(TARGETS ${PLUGIN}
    DESTINATION "${INSTALL_DIR}/lib/lxqt-panel")
install(FILES ${CMAKE_BINARY_DIR}/${PLUGIN}.desktop
    DESTINATION "${INSTALL_DIR}/share/lxqt/lxqt-panel")

# Google Test
include(ExternalProject)
ExternalProject_Add(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
    PREFIX ${CMAKE_BINARY_DIR}/googletest
    INSTALL_COMMAND ""
)
ExternalProject_Get_Property(googletest binary_dir)
link_directories(${binary_dir}/lib)
ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/googletest/include)
include_directories(${source_dir}/googlemock/include)

add_executable(test_ident
    ident.h tests/test_ident.cpp)

target_link_libraries(test_ident gtest gtest_main Qt6::Core)
add_test(NAME test_ident COMMAND test_ident)

set(TESTS test_ident)

set(TESTS_MARKER_FILE run_tests)

add_custom_command(
    OUTPUT ${TESTS_MARKER_FILE}
    COMMAND ${CMAKE_CTEST_COMMAND} -j --verbose --output-on-failure
    COMMAND ${CMAKE_COMMAND} -E touch ${TESTS_MARKER_FILE}
    DEPENDS ${TESTS})
add_custom_target(check ALL DEPENDS run_tests)
