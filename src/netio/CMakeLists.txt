add_library(netio "")

target_sources(netio PRIVATE buffers.cpp PRIVATE config.cpp PRIVATE socket.cpp)
