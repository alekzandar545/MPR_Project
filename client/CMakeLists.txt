project(mpr)
add_executable(client client.cpp)

# Link shared code if you add any .cpp files in src/
target_sources(client PRIVATE
    ${CMAKE_SOURCE_DIR}/src/matrix.cpp
)
