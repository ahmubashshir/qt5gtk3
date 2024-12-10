set(QtPluginDirs_FOUND FALSE)
if(NOT QtPluginDirs_FOUND)
	find_package(Qt5Core REQUIRED QUIET)
	get_target_property(REAL_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE} IMPORTED_LOCATION)
	execute_process(COMMAND "${REAL_QMAKE_EXECUTABLE}" -query QT_INSTALL_PLUGINS
		        OUTPUT_VARIABLE QtPluginDirs_DIR
		        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(QtPluginDirs_FOUND TRUE)
endif()

if(QtPluginDirs_FOUND AND NOT QtPluginDirs_DIR STREQUAL "${QT_PLUGINS_DIR}")
	set(QT_PLUGINS_DIR ${QtPluginDirs_DIR} CACHE PATH "Qt Plugin Base Path")
	find_package_message(QtPluginDirs "Found QtPluginDirs: ${QT_PLUGINS_DIR}" "[${QT_PLUGINS_DIR}][${QtPluginDirs_DIR}]")
endif()
