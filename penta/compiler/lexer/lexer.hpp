#pragma once
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>

enum class pTokenType {
    // Literals
    Int,                // 123           
    Float,              // 123.456
    Str,                // "hello"
    Ident,              // my_variable

    // Keywords
    Set,
    Func,
    If,
    Else,
    True,
    False,

    // Operators
    Assign,             // =
    Equal,              // =
    NotEqual,           // !=   
    Not,                // !
    LessThan,           // <    
    GreaterThan,        // >
    LessEqual,          // <=
    GreaterEqual,       // >=
    Plus,               // +
    Minus,              // -
    Multiply,           // *
    Divide,             // /

    // Symbols
    LeftParenthese,     // (
    RightParenthese,    // )
    LeftBrace,          // {
    RightBrace,         // }
    Semicolon,          // ;
    Comma,              // ,
    Dot,                // .
    
    // Special
    End_of_Line
};

std :: string to_string(pTokenType penta);

using Value  = std :: variant < std :: monostate, long long, double, std:: string >;

struct Token {
    pTokenType type;
    Value value;
    int line;
    int col;
};

class LexError : public std :: runtime_error {
public:
    using std :: runtime_error :: runtime_error;
};

class Lexer {
public:
    explicit Lexer(std :: string src) : src_(std:: move(src)) {}
    std :: vector < Token > tokenize();

private:
    std :: string src_;
    size_t pos_ = 0;
    int line_ = 1;
    int col_ = 1;

    char peek(size_t offset = 0) const;
    char advance();
    bool at_end() const { return pos_ >= src_.size(); }
    [[noreturn]] void error(const std::string& msg) const;

    Token read_word();
    Token read_number();
    Token read_string();
    Token read_symbol();

};
