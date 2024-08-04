add_executable(test_uri test_uri.cpp)
target_link_libraries(test_uri PRIVATE uri)

add_test(NAME TestUri COMMAND "$<TARGET_FILE:test_uri>")
