/**
 * @brief Extracts structured data from parsed AST nodes.
 * 
 * Traverses an Abstract Syntax Tree and extracts matched values for
 * grammar symbols, storing them in a structured format for easy access.
 * Provides configurable extraction with filtering and formatting options.
 * Provides C++98 compatible implementation.
 */

#ifndef DATA_EXTRACTOR_HPP
#define DATA_EXTRACTOR_HPP

#include "AST.hpp"
#include "ExtractedData.hpp"
#include <set>
#include <vector>

class DataExtractor {
public:
    /**
     * @brief Constructs a DataExtractor with default settings.
     */
    DataExtractor();

    /**
     * @brief Extracts data from an AST root node.
     * @param root The root node of the AST to extract data from
     * @return ExtractedData structure containing all matched symbols
     */
    ExtractedData extract(ASTNode* root);

    /**
     * @brief Sets specific symbols to extract (filters output).
     * @param symbols Vector of symbol names to extract (e.g., "<command>", "<params>")
     * If empty, extracts all symbols found.
     */
    void setSymbols(const std::vector<std::string>& symbols);

    /**
     * @brief Sets whether to include terminal symbols in extraction.
     * @param include true to include terminals, false to exclude them (default: false)
     */
    void includeTerminals(bool include);

    /**
     * @brief Sets whether to flatten repetition nodes.
     * @param flatten true to flatten repetitions into parent level, false to keep structure (default: false)
     */
    void flattenRepetitions(bool flatten);

    /**
     * @brief Resets all configuration to defaults.
     */
    void resetConfig();

private:
    /**
     * @brief Recursively visits AST nodes to extract data.
     * @param node Current node being visited
     * @param out Output data structure to populate
     */
    void visit(ASTNode* node, ExtractedData& out);

    /**
     * @brief Checks if a string represents a non-terminal symbol.
     * @param s String to check
     * @return true if string is a non-terminal (enclosed in angle brackets)
     */
    bool isNonTerminal(const std::string& s) const {
        return s.size() >= 2 && s[0] == '<' && s[s.size()-1] == '>';
    }

    /**
     * @brief Checks if a string represents a terminal symbol.
     * @param s String to check
     * @return true if string is a terminal (not enclosed in angle brackets and not structural)
     */
    bool isTerminal(const std::string& s) const {
        return !isNonTerminal(s) && !isStructural(s);
    }

    /**
     * @brief Checks if a string represents a structural element.
     * @param s String to check
     * @return true if string is structural (<seq>, <alt>, <opt>, <rep>)
     */
    bool isStructural(const std::string& s) const {
        return s == "<seq>" || s == "<alt>" || s == "<opt>" || s == "<rep>";
    }

    /**
     * @brief Checks if a symbol should be extracted based on configuration.
     * @param symbol Symbol to check
     * @return true if symbol should be extracted
     */
    bool shouldExtract(const std::string& symbol) const;

    // Configuration options
    std::set<std::string> targetSymbols;  ///< Specific symbols to extract (empty = all)
    bool extractTerminals;                ///< Whether to extract terminal symbols
    bool flattenReps;                     ///< Whether to flatten repetition structures
};

#endif
