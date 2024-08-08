add_library(http1 "")

target_sources(http1 PRIVATE messages.cpp PRIVATE reader.cpp PRIVATE writer.cpp)
