cmake_minimum_required(VERSION 3.14)

project(ircserv
    VERSION 1.0
    LANGUAGES CXX
)

# Norme C++
# set(CMAKE_CXX_STANDARD 98)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# Flags (équivalent -Wall -Wextra -Werror)
add_compile_options(-Wall -Wextra -Wno-unused-private-field -Wno-non-virtual-dtor -Wno-terminate)

# Récupère tous les .cpp
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

# Exécutable
add_executable(ircserv ${SRC_FILES})

# Includes PUBLICS
target_include_directories(ircserv
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)