set (TARGET_NAME SimpleMultiplayerShooter)
define_source_files ()
setup_main_executable ()
target_link_libraries (SimpleMultiplayerShooter SharedBetweenServerAndGame)
