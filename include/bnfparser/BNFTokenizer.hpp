#ifndef BNF_TOKENIZER_HPP
#define BNF_TOKENIZER_HPP

#include <string>
#include <vector>

/**
 * @brief Represents a token in BNF grammar text.
 * 
 * Tokens are the basic lexical units extracted from BNF input text,
 * including symbols, terminals, operators, and structural elements.
 */
struct Token {
    /**
     * @brief Token type enumeration.
     * 
     * Defines all possible token types that can appear in BNF grammar.
     */
    enum Type {
        TOK_SYMBOL,     ///< Non-terminal symbol like <expr>
        TOK_TERMINAL,   ///< Terminal string literal 'text' or "text"
        TOK_LBRACE,     ///< Left brace {
        TOK_RBRACE,     ///< Right brace }
        TOK_LBRACKET,   ///< Left bracket [
        TOK_RBRACKET,   ///< Right bracket ]
        TOK_LPAREN,     ///< Left parenthesis (
        TOK_RPAREN,     ///< Right parenthesis )
        TOK_PIPE,       ///< Pipe operator |
        TOK_ELLIPSIS,   ///< Ellipsis ... for ranges
        TOK_CARET,      ///< Caret ^ for exclusion
        TOK_HEX,        ///< Hexadecimal literal 0xNN
        TOK_WORD,       ///< Simple word token
        TOK_END         ///< End of input marker
    };

    Type type;          ///< The type of this token
    std::string value;  ///< The textual value of this token

    /**
     * @brief Constructs a token with the given type and value.
     * @param t The token type
     * @param v The token value
     */
    Token(Type t, const std::string& v);
};

/**
 * @brief Tokenizer for BNF grammar text.
 * 
 * Breaks down BNF input text into a sequence of tokens that can be
 * consumed by a parser. Supports symbols, terminals, operators, and
 * structural elements like braces and brackets.
 */
class BNFTokenizer {
public:
    /**
     * @brief Constructs a tokenizer for the given input text.
     * @param input The BNF text to tokenize
     */
    BNFTokenizer(const std::string& input);

    /**
     * @brief Consumes and returns the next token from the input.
     * @return The next token, or TOK_END if no more tokens
     */
    Token next();

    /**
     * @brief Peeks at the next token without consuming it.
     * @return The next token that would be returned by next()
     */
    Token peek();

private:
    std::string text;   ///< Input text being tokenized
    size_t pos;         ///< Current position in the input text

    /**
     * @brief Skips whitespace characters at the current position.
     */
    void skipSpaces();

    /**
     * @brief Parses a symbol token of the form <name>.
     * @return Symbol token
     */
    Token parseSymbol();

    /**
     * @brief Parses a terminal token enclosed in quotes.
     * @return Terminal token
     */
    Token parseTerminal();

    /**
     * @brief Parses a simple word token.
     * @return Word token
     */
    Token parseWord();

    /**
     * @brief Parses a hexadecimal literal token (0xNN).
     * @return Hex token
     */
    Token parseHex();

    /**
     * @brief Checks if the next characters form an ellipsis (...).
     * @return true if ellipsis is found
     */
    bool isEllipsis() const;
};

#endif
