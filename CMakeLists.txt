project(kndesktop)

find_package(KDE4 REQUIRED)
find_package(LibKonq REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

########## kndesktop ##########

set(kndesktop_SRCS
    container.cpp
    iconview.cpp
    kndesktop.cpp
    main.cpp
)

kde4_add_executable(kndesktop ${kndesktop_SRCS})

target_link_libraries(kndesktop
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KFILE_LIBS}
    ${KDE4_KIO_LIBS}
    ${LIBKONQ_LIBRARY}
    X11
)

install(TARGETS kndesktop DESTINATION ${BIN_INSTALL_DIR})
# install(FILES kndesktop.desktop DESTINATION ${SERVICES_INSTALL_DIR})

