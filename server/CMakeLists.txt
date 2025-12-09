project(mpr)
add_executable(server server.cpp)

# Link shared code if you add any .cpp files in src/
target_sources(server PRIVATE
    ${CMAKE_SOURCE_DIR}/src/matrix.cpp
)
