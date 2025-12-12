#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <sstream>

// #define ENABLE_DEBUG

/**
 * @brief Debug utilities for the BNF interpreter.
 * 
 * Provides conditional compilation debug macros that can be enabled
 * or disabled at compile time. Uses std::ostringstream for safe
 * evaluation of streamable values in C++98 compatible way.
 */
#ifdef ENABLE_DEBUG

#define DEBUG_MSG(msg) \
    do { \
        std::ostringstream __dbg_ss; \
        __dbg_ss << msg; \
        std::cerr << "[DEBUG] " << __dbg_ss.str() << std::endl; \
    } while(0)

#define DEBUG_VAL(name, val) \
    do { \
        std::ostringstream __dbg_ss; \
        __dbg_ss << (val); \
        std::cerr << "[DEBUG] " << (name) << "=" << __dbg_ss.str() << std::endl; \
    } while(0)

#else

#define DEBUG_MSG(msg) do { } while(0)
#define DEBUG_VAL(name, val) do { } while(0)

#endif // ENABLE_DEBUG

#endif // DEBUG_HPP
