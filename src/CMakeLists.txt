add_library(miner MODULE miner-plugin.c)

set_target_properties(miner PROPERTIES PREFIX "")

install(TARGETS miner LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/multiminer/plugins)
