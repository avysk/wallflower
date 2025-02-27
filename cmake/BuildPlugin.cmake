MACRO (BUILD_LXQT_PLUGIN NAME)
    set(PROGRAM "lxqt-panel")

    set(PROG_SHARE_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/lxqt/${PROGRAM})
    set(PLUGIN_SHARE_DIR ${PROG_SHARE_DIR}/${NAME})

    file (GLOB CONFIG_FILES resources/*.conf)

    set(QTX_LIBRARIES Qt6::Widgets)
    set(SRC ${HEADERS} ${SOURCES})
    add_library(${NAME} MODULE ${SRC}) # build dynamically loadable modules
    target_link_libraries(${NAME} ${QTX_LIBRARIES} lxqt)

    install(TARGETS ${NAME} DESTINATION ${PLUGIN_DIR})
    install(FILES ${CONFIG_FILES}  DESTINATION ${PLUGIN_SHARE_DIR})

ENDMACRO(BUILD_LXQT_PLUGIN)
