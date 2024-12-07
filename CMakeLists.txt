cmake_minimum_required(VERSION 3.16)
project(DSA_Project)

set(CMAKE_CXX_STANDARD 17)

# Set the path to SFML
set(SFML_DIR "C:/SFML/lib/cmake/SFML")

# Find SFML components
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Add your source files
add_executable(DSA_Project main.cpp)

# Link SFML libraries
target_link_libraries(DSA_Project sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
