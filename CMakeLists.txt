cmake_minimum_required(VERSION 3.16)

project(baka-mplayer
    VERSION 2.0.4
    DESCRIPTION "A free and open source, cross-platform, libmpv based multimedia player"
    HOMEPAGE_URL "https://github.com/u8sand/Baka-MPlayer"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(BAKA_SETTINGS_FILE "bakamplayer" CACHE STRING "Settings file name (without extension)")
set(BAKA_LANG "" CACHE STRING "Force a language code instead of using the system locale")

include(GNUInstallDirs)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets Network Svg OpenGL OpenGLWidgets LinguistTools)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MPV REQUIRED IMPORTED_TARGET mpv)

qt_standard_project_setup()

set(BAKA_SOURCES
    src/main.cpp
    src/bakaengine.cpp
    src/bakacommands.cpp
    src/mpvhandler.cpp
    src/updatemanager.cpp
    src/gesturehandler.cpp
    src/overlayhandler.cpp
    src/util.cpp
    src/settings.cpp
    src/overlay.cpp
    src/versions/2_0_3.cpp
    src/widgets/customlabel.cpp
    src/widgets/customlineedit.cpp
    src/widgets/customslider.cpp
    src/widgets/customsplitter.cpp
    src/widgets/dimdialog.cpp
    src/widgets/indexbutton.cpp
    src/widgets/mpvwidget.cpp
    src/widgets/openbutton.cpp
    src/widgets/playlistwidget.cpp
    src/widgets/seekbar.cpp
    src/ui/aboutdialog.cpp
    src/ui/inputdialog.cpp
    src/ui/jumpdialog.cpp
    src/ui/keydialog.cpp
    src/ui/locationdialog.cpp
    src/ui/mainwindow.cpp
    src/ui/preferencesdialog.cpp
    src/ui/screenshotdialog.cpp
    src/ui/updatedialog.cpp
    src/bakaengine.h
    src/gesturehandler.h
    src/mpvhandler.h
    src/mpvtypes.h
    src/overlay.h
    src/overlayhandler.h
    src/recent.h
    src/settings.h
    src/updatemanager.h
    src/util.h
    src/widgets/customlabel.h
    src/widgets/customlineedit.h
    src/widgets/customslider.h
    src/widgets/customsplitter.h
    src/widgets/dimdialog.h
    src/widgets/indexbutton.h
    src/widgets/mpvwidget.h
    src/widgets/openbutton.h
    src/widgets/playlistwidget.h
    src/widgets/seekbar.h
    src/ui/aboutdialog.h
    src/ui/inputdialog.h
    src/ui/jumpdialog.h
    src/ui/keydialog.h
    src/ui/locationdialog.h
    src/ui/mainwindow.h
    src/ui/preferencesdialog.h
    src/ui/screenshotdialog.h
    src/ui/updatedialog.h
    src/ui/aboutdialog.ui
    src/ui/inputdialog.ui
    src/ui/jumpdialog.ui
    src/ui/keydialog.ui
    src/ui/locationdialog.ui
    src/ui/mainwindow.ui
    src/ui/preferencesdialog.ui
    src/ui/screenshotdialog.ui
    src/ui/updatedialog.ui
    src/rsclist.qrc
)

if(WIN32)
    list(APPEND BAKA_SOURCES src/platform/win.cpp src/baka-mplayer.rc)
elseif(APPLE)
    list(APPEND BAKA_SOURCES src/platform/osx.cpp src/img/logo.icns)
    set_source_files_properties(src/img/logo.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
else()
    list(APPEND BAKA_SOURCES src/platform/linux.cpp)
endif()

qt_add_executable(baka-mplayer WIN32 MACOSX_BUNDLE ${BAKA_SOURCES})

target_include_directories(baka-mplayer PRIVATE src)
set_target_properties(baka-mplayer PROPERTIES AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/src/ui)
target_link_libraries(baka-mplayer PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets
    PkgConfig::MPV)

if(UNIX AND NOT APPLE)
    find_package(X11)
    if(X11_FOUND)
        target_link_libraries(baka-mplayer PRIVATE X11::X11)
        target_compile_definitions(baka-mplayer PRIVATE BAKA_HAVE_X11)
    endif()
endif()

target_compile_definitions(baka-mplayer PRIVATE
    BAKA_MPLAYER_VERSION="${PROJECT_VERSION}"
    SETTINGS_FILE="${BAKA_SETTINGS_FILE}"
    BAKA_MPLAYER_LANG_PATH=":/translations")
if(BAKA_LANG)
    target_compile_definitions(baka-mplayer PRIVATE BAKA_MPLAYER_LANG="${BAKA_LANG}")
endif()

# translations are compiled and embedded into the binary
file(GLOB BAKA_TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/translations/*.ts)
qt_add_translations(baka-mplayer
    TS_FILES ${BAKA_TS_FILES}
    RESOURCE_PREFIX /translations
    LUPDATE_OPTIONS -no-obsolete -locations none)

set_target_properties(baka-mplayer PROPERTIES
    OUTPUT_NAME $<IF:$<BOOL:${APPLE}>,Baka-MPlayer,baka-mplayer>
    MACOSX_BUNDLE_BUNDLE_NAME "Baka MPlayer"
    MACOSX_BUNDLE_GUI_IDENTIFIER "net.u8sand.baka-mplayer"
    MACOSX_BUNDLE_ICON_FILE logo.icns
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})

install(TARGETS baka-mplayer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(UNIX AND NOT APPLE)
    install(FILES etc/baka-mplayer.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES etc/logo/baka-mplayer.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
    install(FILES etc/logo/baka-mplayer.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
    install(FILES DOCS/baka-mplayer.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
    install(FILES DOCS/baka-mplayer.1.gz DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
    install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/baka-mplayer)
endif()
