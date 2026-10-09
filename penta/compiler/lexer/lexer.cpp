#include "lexer.hpp"
#include <cctype>
#include <unordered_map>


static const std :: unordered_map < std :: string, pTokenType > KEYWORDS = {
    {"set", pTokenType :: Set},
    {"func", pTokenType :: Func},
    {"if", pTokenType :: If},
    {"else", pTokenType :: Else},
    {"True", pTokenType :: True},
    {"False", pTokenType :: False},
};

static const std :: unordered_map < char, pTokenType> SINGLE = {
    {'=', pTokenType :: Assign},

    {'!', pTokenType :: Not},
    {'<', pTokenType ::LessThan },
    {'>', pTokenType :: GreaterThan},
    {'+', pTokenType :: Plus},
    {'-', pTokenType :: Minus},
    {'*', pTokenType :: Multiply},
    {'/', pTokenType :: Divide},
    {'(', pTokenType :: LeftParenthese},
    {')', pTokenType :: RightParenthese},
    {'{', pTokenType :: LeftBrace},
    {'}', pTokenType :: RightBrace},
    {';', pTokenType :: Semicolon},
    {',', pTokenType :: Comma},
    {'.', pTokenType :: Dot},
};

static const std :: unordered_map < std :: string, pTokenType > DOUBLE = {
    {"==", pTokenType :: Equal },
    {"!=", pTokenType :: NotEqual},
    {"<=", pTokenType :: LessEqual},
    {">=", pTokenType :: GreaterEqual},
};

std::string to_string(pTokenType penta) {

    switch (penta) {
        case pTokenType :: Int: return "INT";
        case pTokenType :: Float: return "FLOAT";
        case pTokenType :: Str: return "STRING";
        case pTokenType :: Ident: return "IDENT";

        case pTokenType :: Set: return "SET";
        case pTokenType :: Func: return "FUNC";
        case pTokenType :: If: return "IF";
        case pTokenType :: Else: return "ELSE";
        case pTokenType :: True: return "TRUE";
        case pTokenType :: False: return "FALSE";

        case pTokenType :: Assign: return "ASSIGN";
        case pTokenType :: Equal: return "EQUAL";
        case pTokenType :: NotEqual: return "NOT_EQUAL";
        case pTokenType :: Not: return "NOT";
        case pTokenType :: LessThan: return "LESS_THAN";
        case pTokenType :: GreaterThan: return "GREATER_THAN";
        case pTokenType :: LessEqual: return "LESS_EQUAL";
        case pTokenType :: GreaterEqual: return "GREATER_EQUAL";
        case pTokenType :: Plus: return "PLUS";
        case pTokenType :: Minus: return "MINUS";
        case pTokenType :: Multiply: return "MULTIPLY";
        case pTokenType :: Divide: return "DIVIDE";

        case pTokenType :: LeftParenthese: return "LEFTPARENTHESE";
        case pTokenType :: RightParenthese: return "RIGHTPARENTHESE";
        case pTokenType :: LeftBrace: return "LEFTBRACE";
        case pTokenType :: RightBrace: return "RIGHTBRACE";
        case pTokenType :: Semicolon: return "SEMICOLON";
        case pTokenType :: Comma: return "COMMA";
        case pTokenType :: Dot: return "DOT";

        case pTokenType :: End_of_Line: return "END_OF_LINE";
    }

    return "?";
}

char Lexer :: peek(size_t offset) const {
    size_t i = pos_ + offset;
    return i < src_.size() ? src_[i] : '\0';
}

char Lexer :: advance() {
    char c = src_[pos_++];
    if (c == '\n') { line_++; col_ = 1; }
    else           { col_++; }
    return c;
}

void Lexer::error(const std::string& msg) const {
    throw LexError("Setir " + std::to_string(line_) + ", sutun " +
                   std::to_string(col_) + ": " + msg);
}

static bool is_alpha(char c) { return std :: isalpha(static_cast<unsigned char>(c)) || c == '_'; }
static bool is_digit(char c) { return std :: isdigit(static_cast<unsigned char>(c)); }
static bool is_alnum(char c) { return is_alpha(c) || is_digit(c); }

std :: vector < Token > Lexer :: tokenize() {
    std :: vector < Token > tokens;
    while (!at_end()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/' && peek(1) == '/') {
            while (!at_end() && peek() != '\n') advance();
        } else if (is_alpha(c)) {
            tokens.push_back(read_word());
        } else if (is_digit(c)) {
            tokens.push_back(read_number());
        } else if (c == '"') {
            tokens.push_back(read_string());
        } else {
            tokens.push_back(read_symbol());
        }
    }
    tokens.push_back({pTokenType :: End_of_Line, std :: monostate{}, line_, col_});
    return tokens;
}

Token Lexer :: read_word() {
    int line = line_, col = col_;
    size_t start = pos_;
    while (is_alnum(peek())) advance();
    std :: string word = src_.substr(start, pos_ - start);

    auto it = KEYWORDS.find(word);
    pTokenType type = (it != KEYWORDS.end()) ? it-> second : pTokenType :: Ident;
    return {type, word, line, col};
}

Token Lexer :: read_number() {
    int line = line_, col = col_;
    size_t start = pos_;
    while (is_digit(peek())) advance();

    if (peek() == '.' && is_digit(peek(1))) {
        advance();
        while (is_digit(peek())) advance();
        return {pTokenType :: Float, std :: stod(src_.substr(start, pos_ - start)), line, col};
    }
    return {pTokenType :: Int, std :: stoll(src_.substr(start, pos_ - start)), line, col};
}

Token Lexer :: read_string() {
    int line = line_, col = col_;
    advance(); 
    std::string out;
    while (true) {
        if (at_end()) error("unbound string");
        char c = advance();
        if (c == '"') break;
        if (c == '\\') {
            if (at_end()) error("unbound string");
            char e = advance();
            switch (e) {
                case 'n':  out += '\n'; break;
                case 't':  out += '\t'; break;
                case '"':  out += '"';  break;
                case '\\': out += '\\'; break;
                default: error(std :: string("unknown escape: \\") + e);
            }
        } else {
            out += c;
        }
    }
    return {pTokenType :: Str, out, line, col};
}

Token Lexer :: read_symbol() {
    int line = line_, col = col_;

    std :: string two{peek(), peek(1)};
    auto d = DOUBLE.find(two);
    if (d != DOUBLE.end()) {
        advance(); advance();
        return {d->second, two, line, col};
    }

    char c = peek();
    auto s = SINGLE.find(c);
    if (s != SINGLE.end()) {
        advance();
        return {s->second, std :: string(1, c), line, col};
    }
    error(std :: string("unknown symbol '") + c + "'");
}