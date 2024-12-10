# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause
set(PTSRCS
    main.cpp
    qgtk3colorscheme_p.h
    qgtk3dialoghelpers.cpp qgtk3dialoghelpers.h

    qgtk3theme.cpp qgtk3theme.h
    qgtk3interface.cpp qgtk3interface_p.h
    qgtk3storage.cpp qgtk3storage_p.h
    qgtk3json.cpp qgtk3json_p.h

	qgtk3dialoghelpers.cpp
)

set(PTLIBS
    PkgConfig::GTK3
    Qt::Core
    Qt::CorePrivate
    Qt::Gui
    Qt::GuiPrivate
    Qt5::ThemeSupport
    Qt5::ThemeSupportPrivate
)

if(ENABLE_DBUS)
    list(APPEND PTSRCS qgtk3portalinterface.cpp)
    list(APPEND PTLIBS Qt::DBus)
endif()

if(ENABLE_X11)
    list(APPEND PTLIBS X11)
endif()

add_library(qt5gtk3 MODULE ${PTSRCS})

target_link_libraries(qt5gtk3 ${PTLIBS})
install(TARGETS qt5gtk3 DESTINATION ${QT_PLUGINS_DIR}/platformthemes)
