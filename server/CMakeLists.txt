#project(mpr)
add_executable(server main.cpp)
target_link_libraries(server PRIVATE ws2_32)


# Link shared code if you add any .cpp files in src/
target_sources(server PRIVATE
    ${CMAKE_SOURCE_DIR}/server/server.cpp
    ${CMAKE_SOURCE_DIR}/src/matrix.cpp
    ${CMAKE_SOURCE_DIR}/src/socket.cpp
    ${CMAKE_SOURCE_DIR}/src/thread_pool.cpp
    ${CMAKE_SOURCE_DIR}/src/logger.cpp
)
