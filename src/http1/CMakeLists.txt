add_library(http1 "")

target_sources(http1 PRIVATE reader.cpp PRIVATE writer.cpp)
