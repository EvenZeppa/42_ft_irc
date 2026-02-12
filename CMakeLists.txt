cmake_minimum_required(VERSION 3.14)

project(ircserv
    VERSION 1.0
    LANGUAGES CXX
)

enable_testing()

# Norme C++
# set(CMAKE_CXX_STANDARD 98)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# Flags (équivalent -Wall -Wextra -Werror)
include(CheckCXXCompilerFlag)

set(CXX_WARN_FLAGS
    -Wall
    -Wextra
    -Wno-unused-private-field
    -Wno-non-virtual-dtor
    -Wno-terminate
)

foreach(flag IN LISTS CXX_WARN_FLAGS)
    string(MAKE_C_IDENTIFIER "${flag}" flag_id)
    set(flag_var "HAS_${flag_id}")
    check_cxx_compiler_flag("${flag}" "${flag_var}")
    if(${flag_var})
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${flag}>)
    endif()
endforeach()

# Récupère tous les .cpp
file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

# Exécutable
add_executable(ircserv ${SRC_FILES})

set(BNFPARSER_BUILD_EXAMPLES OFF CACHE BOOL "Disable BNFParser examples" FORCE)
set(BNFPARSER_BUILD_TESTS OFF CACHE BOOL "Disable BNFParser tests" FORCE)
add_subdirectory(third_party/BNFParserLib EXCLUDE_FROM_ALL)
target_link_libraries(ircserv PRIVATE bnf)

if(APPLE)
    add_subdirectory(third_party/epoll-shim)
    target_link_libraries(ircserv PRIVATE epoll-shim)
    target_compile_definitions(ircserv PRIVATE EPOLL_SHIM_DISABLE_WRAPPER_MACROS)
endif()

# Includes PUBLICS
target_include_directories(ircserv
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party/BNFParserLib/include
)

# Tests
file(GLOB_RECURSE TEST_FILES
    test/*.cpp
)

set(TEST_SRC_FILES ${SRC_FILES})
list(FILTER TEST_SRC_FILES EXCLUDE REGEX "src/main\\.cpp$")

add_executable(test_runner ${TEST_FILES} ${TEST_SRC_FILES})
target_link_libraries(test_runner PRIVATE bnf)
if(APPLE)
    target_link_libraries(test_runner PRIVATE epoll-shim)
    target_compile_definitions(test_runner PRIVATE EPOLL_SHIM_DISABLE_WRAPPER_MACROS)
endif()

target_include_directories(test_runner
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party/BNFParserLib/include
)

add_test(NAME irc_tests COMMAND test_runner)

add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_runner
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Remove CMake-generated build artifacts (like make fclean)
add_custom_target(fclean
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_BINARY_DIR}/CMakeFiles
        ${CMAKE_BINARY_DIR}/CMakeCache.txt
        ${CMAKE_BINARY_DIR}/cmake_install.cmake
        ${CMAKE_BINARY_DIR}/Makefile
        ${PROJECT_SOURCE_DIR}/build
)

# Clean and rebuild in one command (like make re)
add_custom_target(re
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_BINARY_DIR}/CMakeFiles
        ${CMAKE_BINARY_DIR}/CMakeCache.txt
        ${CMAKE_BINARY_DIR}/cmake_install.cmake
        ${CMAKE_BINARY_DIR}/Makefile
        ${PROJECT_SOURCE_DIR}/build
    COMMAND ${CMAKE_COMMAND} -S ${PROJECT_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
)