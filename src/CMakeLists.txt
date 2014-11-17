add_library(miner MODULE
    miner-plugin.c
    miner-command.c miner-command.h
    miner-device.c miner-device.h
    miner-network.c miner-network.h
    utility-network.c utility-network.h
    utility-application.c utility-application.h
    api-rpc.c api-rpc.h)

set_target_properties(miner PROPERTIES PREFIX "")

target_link_libraries(miner jansson)

install(TARGETS miner LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/multiminer/plugins)
