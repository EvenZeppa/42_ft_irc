#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <iostream>

/**
 * @brief Abstract Syntax Tree node for parsed BNF expressions.
 * 
 * Represents a node in the parse tree generated from BNF grammar rules.
 * Each node contains a symbol name, matched text, and child nodes forming
 * a hierarchical structure representing the parsed input.
 */
struct ASTNode {
    std::string symbol;                 ///< Symbol name or node type
    std::string matched;                ///< Text matched by this node
    std::vector<ASTNode*> children;     ///< Child nodes in the parse tree

    /**
     * @brief Constructs an AST node with the given symbol name.
     * @param s The symbol name for this node
     */
    ASTNode(const std::string& s);

    /**
     * @brief Destructor that recursively deletes all child nodes.
     */
    ~ASTNode();
};

/**
 * @brief Prints the AST structure in a readable hierarchical format.
 * @param node The root node to print
 * @param indent Indentation level for formatting (default: 0)
 */
void printAST(const ASTNode* node, int indent = 0);

#endif
