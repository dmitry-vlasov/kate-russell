project(katerussellplugin)

add_definitions(-DTRANSLATION_DOMAIN=\"katerussellplugin\")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "-O0 -ggdb3")
set(CXXFLAGS "-O0 -ggdb3")
set(CPPFLAGS "-O0 -ggdb3")

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(syntax)

set (katerussell_SRC 
	ProjectConfigTab.cpp
	RussellConfigPage.cpp
	
	LatexToUnicode.cpp
	
	commands.cpp
	Launcher.cpp
	Execute.cpp
	Enums.cpp
	
	Navigation.cpp
	Outline.cpp
	Structure.cpp
	TypeSystem.cpp
	Proof.cpp
	View.cpp
	Plugin.cpp
	HypNode.cpp
)

set (syntaxhighlighting
	syntax/rus.xml
	syntax/smm.xml
	syntax/mm.xml
)

ki18n_wrap_ui(katerussell_SRC BottomTab.ui RussellConfig.ui ProjectsTab.ui HypNode.ui)

qt5_add_resources(katerussell_SRC plugin.qrc)

add_library(katerussellplugin MODULE ${katerussell_SRC})

kcoreaddons_desktop_to_json (katerussellplugin katerussellplugin.desktop)

target_link_libraries (katerussellplugin 
    KF5::TextEditor
    KF5::Parts
    KF5::IconThemes
    KF5::I18n
    KF5::Service
    stdc++fs
)

install(TARGETS katerussellplugin DESTINATION ${PLUGIN_INSTALL_DIR}/ktexteditor )
install(TARGETS katerussellplugin DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ktexteditor )

