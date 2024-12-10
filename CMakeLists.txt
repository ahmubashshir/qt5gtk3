cmake_minimum_required(VERSION 3.16)

project(Qt5Gtk3ThemePlugin
    VERSION "6.8.1"
    DESCRIPTION "Qt5 backport of Qt6 Gtk3 platformtheme and accompanying styleplugin"
    HOMEPAGE_URL "https://github.com/ahmubashshir/qt5gtk3"
    LANGUAGES CXX C ASM
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
set(CMAKE_AUTOMOC ON)
set(QT_MIN_VERSION "5.15.2")

include(GNUInstallDirs)
include(FeatureSummary)

option(ENABLE_X11  "Enable X11 Support" ON)
option(ENABLE_DBUS "Enable DBus Integration" ON)

find_package(GTK3 REQUIRED)
find_package(Qt5Core CONFIG COMPONENTS Private REQUIRED)
find_package(Qt5Gui COMPONENTS Private REQUIRED)
find_package(Qt5ThemeSupport COMPONENTS Private REQUIRED)

add_compile_definitions(GDK_VERSION_MIN_REQUIRED=GDK_VERSION_3_6)

if(ENABLE_X11)
    add_compile_definitions(ENABLE_X11)
    find_package(X11 REQUIRED)
endif()

if(ENABLE_DBUS)
    find_package(Qt5DBus REQUIRED)
endif()

get_target_property(REAL_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} IMPORTED_LOCATION)
execute_process(COMMAND "${REAL_QMAKE_EXECUTABLE}" -query QT_INSTALL_PLUGINS
            OUTPUT_VARIABLE QT_PLUGINS_DIR
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT QT_PLUGINS_DIR)
	message(FATAL_ERROR "Couldn't determine QT_PLUGINS_DIR.")
else()
	message(STATUS "Found QT_PLUGINS_DIR: ${QT_PLUGINS_DIR}")
endif()
add_subdirectory(platformtheme)
