project(kndesktop)

find_package(KDE4 REQUIRED)
find_package(LibKonq REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

########## libkrootpixmap ##########

set(krootpixmap_SRCS
    krootpixmap.cpp
    krootpixmaploader.cpp
)

kde4_add_library(krootpixmap SHARED ${krootpixmap_SRCS})

target_link_libraries(krootpixmap ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} X11)
set_target_properties(krootpixmap PROPERTIES VERSION 0.0.1 SOVERSION 0)

install(TARGETS krootpixmap ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
    krootpixmap.h
    krootpixmap_export.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/krootpixmap
    COMPONENT Devel
)

########## kndesktop ##########

set(kndesktop_SRCS
    container.cpp
    iconview.cpp
    kndesktop.cpp
    main.cpp
    rootpixmapserver.cpp
)

qt4_add_dbus_adaptor(kndesktop_SRCS org.kde.kndesktop.IconView.xml iconview.h IconView)

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

