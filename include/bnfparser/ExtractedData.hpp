/**
 * @brief Data structure for storing extracted values from parsed AST.
 * 
 * Provides a container for storing matched values from BNF grammar symbols
 * during AST traversal. Uses C++98 compatible standard library containers.
 */

#ifndef EXTRACTED_DATA_HPP
#define EXTRACTED_DATA_HPP

#include <string>
#include <map>
#include <vector>

struct ExtractedData {
    // Map: "<symbol>" → all matched occurrences (C++98 compatible)
    std::map<std::string, std::vector<std::string> > values;

    /**
     * @brief Tests if a symbol exists in the extracted data.
     * @param sym Symbol name to check
     * @return true if symbol exists, false otherwise
     */
    bool has(const std::string& sym) const {
        return values.find(sym) != values.end();
    }

    /**
     * @brief Gets the first value for a symbol (often useful).
     * @param sym Symbol name to get value for
     * @return First value if exists, empty string otherwise
     */
    std::string first(const std::string& sym) const {
        std::map<std::string, std::vector<std::string> >::const_iterator it = values.find(sym);
        if (it == values.end() || it->second.empty())
            return "";
        return it->second[0];
    }

    /**
     * @brief Gets all values for a symbol.
     * @param sym Symbol name to get values for
     * @return Vector of all values, empty if symbol doesn't exist
     */
    std::vector<std::string> all(const std::string& sym) const {
        std::map<std::string, std::vector<std::string> >::const_iterator it = values.find(sym);
        if (it == values.end())
            return std::vector<std::string>();
        return it->second;
    }

    /**
     * @brief Gets the count of values for a symbol.
     * @param sym Symbol name to count values for
     * @return Number of values for the symbol
     */
    size_t count(const std::string& sym) const {
        std::map<std::string, std::vector<std::string> >::const_iterator it = values.find(sym);
        if (it == values.end())
            return 0;
        return it->second.size();
    }
};

#endif
