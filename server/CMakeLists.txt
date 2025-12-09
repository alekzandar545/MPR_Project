#project(mpr)
add_executable(server server.cpp)
target_link_libraries(server PRIVATE ws2_32)


# Link shared code if you add any .cpp files in src/
target_sources(server PRIVATE
    ${CMAKE_SOURCE_DIR}/src/matrix.cpp
    ${CMAKE_SOURCE_DIR}/src/socket.cpp
)
