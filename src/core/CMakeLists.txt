add_library(core "")

# TODO: add PRIVATE producer.cpp PRIVATE server.cpp
target_sources(core PRIVATE worker.cpp)
