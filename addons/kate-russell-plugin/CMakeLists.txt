project(katerussellplugin)

add_definitions(-DTRANSLATION_DOMAIN=\"katerussellplugin\")

#include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "-O0 -ggdb3")
set(CXXFLAGS "-O0 -ggdb3")
set(CPPFLAGS "-O0 -ggdb3")

add_subdirectory(icons)
add_subdirectory(syntax)

set (katerussell_SRC 
	plugin_kate_russell_config_Combinations.cpp
	plugin_kate_russell_config_Config.cpp
	plugin_kate_russell_config_Indexes.cpp
	plugin_kate_russell_config_Session.cpp
	plugin_kate_russell_config_Ui.cpp
	
	plugin_kate_russell_ErrorParser.cpp
	plugin_kate_russell_Icon.cpp
	plugin_kate_russell_LatexToUnicode.cpp
	
	plugin_kate_russell_mdl_Client.cpp
	plugin_kate_russell_mdl_Launcher.cpp
	plugin_kate_russell_mdl_Server.cpp
	
	plugin_kate_russell_Navigation.cpp
	plugin_kate_russell_Outline.cpp
	plugin_kate_russell_Structure.cpp
	plugin_kate_russell_TypeSystem.cpp
	plugin_kate_russell_Proof.cpp
	plugin_kate_russell_View.cpp
	plugin_kate_russell_Plugin.cpp
)

set (syntaxhighlighting
	syntax/rus.xml
	syntax/smm.xml
	syntax/mm.xml
)

ki18n_wrap_ui(katerussell_SRC bottom.ui)

qt5_add_resources(katerussell_SRC plugin.qrc)

add_library(katerussellplugin MODULE ${katerussell_SRC})

kcoreaddons_desktop_to_json (katerussellplugin katerussellplugin.desktop)

target_link_libraries (katerussellplugin 
    KF5::TextEditor
    KF5::Parts
    KF5::IconThemes
    KF5::I18n
    KF5::Service
)

install (TARGETS katerussellplugin DESTINATION ${PLUGIN_INSTALL_DIR}/ktexteditor)

#install (FILES ui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kate/plugins/kate-russell)
#install (FILES katerussellplugin.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})




