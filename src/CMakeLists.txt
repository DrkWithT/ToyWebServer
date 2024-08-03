add_executable(toyserver main.cpp)

add_subdirectory(uri)
add_subdirectory(netio)
add_subdirectory(http1)
# add_subdirectory(core)
# add_subdirectory(app)

target_link_libraries(toyserver PRIVATE uri PRIVATE netio PRIVATE http1)
