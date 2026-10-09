#include "compiler/lexer/lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char** argv) {
    std :: string code;
    if (argc > 1) {
        std :: ifstream f(argv[1]);
        if (!f) { std :: cerr << "File is not opened.\n"; return 1; }
        std :: stringstream ss; ss << f.rdbuf();
        code = ss.str();
    } else {
        code = R"(
set name = "Araz Omarov";   // str
set height = 1.73;
set age = i("What is your name: ");
if (age > 3) {
    o("child");
} else if (age < 18) {
    o("teen");
}
)";
    }
    try {
        for (const Token& pTokenType : Lexer(code).tokenize()) {
            std :: cout << pTokenType.line << ":" << pTokenType.col << "\t" << to_string(pTokenType.type);
            if (auto* s = std :: get_if< std :: string > (&pTokenType.value))   std :: cout << "\t" << *s;
            else if (auto* i = std :: get_if< long long > (&pTokenType.value)) std :: cout << "\t" << *i;
            else if (auto* d = std :: get_if< double > (&pTokenType.value))    std :: cout << "\t" << *d;
            std :: cout << "\n";
        }
    } catch (const LexError& e) {
        std :: cerr << "Lexer error: " << e.what() << "\n";
        return 1;
    }
}

