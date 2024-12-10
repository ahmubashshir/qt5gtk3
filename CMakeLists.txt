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
include(tristate)

include(GNUInstallDirs)
include(FeatureSummary)

tristate(ENABLE_X11 "Enable X11 support")
tristate(ENABLE_DBUS "Enable DBus Integration")

find_package(GTK3 REQUIRED)
find_package(Qt5Core CONFIG COMPONENTS Private REQUIRED)
find_package(Qt5Gui COMPONENTS Private REQUIRED)
find_package(Qt5ThemeSupport COMPONENTS Private REQUIRED)
find_package(QtPluginDirs)

add_compile_definitions(GDK_VERSION_MIN_REQUIRED=GDK_VERSION_3_6)

tristate_package(ENABLE_X11 X11 DEFINE ENABLE_X11)
tristate_package(ENABLE_DBUS Qt5DBus)

add_subdirectory(platformtheme)
