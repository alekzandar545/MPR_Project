#project(mpr)
add_executable(client main.cpp)
target_link_libraries(client PRIVATE ws2_32)

# Link shared code if you add any .cpp files in src/
target_sources(client PRIVATE
    ${CMAKE_SOURCE_DIR}/client/client.cpp
    ${CMAKE_SOURCE_DIR}/src/matrix.cpp
    ${CMAKE_SOURCE_DIR}/src/socket.cpp
    ${CMAKE_SOURCE_DIR}/src/thread_pool.cpp
    ${CMAKE_SOURCE_DIR}/src/logger.cpp
)

