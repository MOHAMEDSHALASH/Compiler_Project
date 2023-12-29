#include <iostream>
#include <vector>
#include <regex>
#include <set>

// Lexical Analysis
std::vector<std::tuple<size_t, std::string, std::string>> lexicalAnalysis(const std::string& inputString) {
    std::vector<std::tuple<size_t, std::string, std::string>> tokens;

    std::vector<std::pair<std::string, std::string>> patterns = {
        { "\\b[a-zA-Z_][a-zA-Z0-9_]*\\b", "IDENTIFIER" },   // Variable names
        { "\\b\\d+\\b", "INTEGER" },                         // Integer literals
        { "\\+", "operator(PLUS)" },                         // Addition operator
        { "\\-", "operator(Minus)" },                        // Subtraction operator
        { "=", "operator(EQUALS)" },                         // Assignment operator
        { "\\*", "ASTERISK" },                                // Multiplication operator
        { "/", "SLASH" },                                    // Division operator
        { ":", "colon" },                                    // colon
        { ",", "comma" },                                    // comma
        { ";", "SEMICOLON" },                                // Semicolon
        { "\n", "NEWLINE" }                                  // Newline character
    };

    for (const auto& pattern : patterns) {
        std::regex regexPattern(pattern.first);
        auto words_begin = std::sregex_iterator(inputString.begin(), inputString.end(), regexPattern);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            tokens.emplace_back(match.position(), match.str(), pattern.second);
        }
    }

    // Sort tokens based on their starting position in the input string
    std::sort(tokens.begin(), tokens.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    return tokens;
}

// Semantic Analysis
class VariableUndefinedError : public std::exception {
public:
    const char* what() const noexcept override {
        return "Variable Undefined Error";
    }
};

class SemanticAnalyzer {
public:
    void visitName(const std::string& id) {
        if (variables.find(id) == variables.end()) {
            throw VariableUndefinedError();
        }
    }

    void visitFunctionDef(const std::string& name) {
        variables.insert(name);
    }

    void analyze(const std::string& input) {
        size_t pos = 0;
        while (pos != std::string::npos) {
            pos = input.find("def", pos);
            if (pos != std::string::npos) {
                size_t start = pos + 3;
                size_t end = input.find("(", start);
                if (end != std::string::npos) {
                    std::string functionName = input.substr(start, end - start);
                    visitFunctionDef(functionName);
                }
                pos = end;
            }
        }
    }

private:
    std::set<std::string> variables;
};

int main() {
    std::string user_input;
    std::cout << "Enter arithmetic expression: ";
    std::getline(std::cin, user_input);

    auto tokens = lexicalAnalysis(user_input);

    for (const auto& token : tokens) {
        std::cout << std::get<2>(token) << ": " << std::get<1>(token) << std::endl;
    }

    // Semantic Analysis
    try {
        SemanticAnalyzer analyzer;
        analyzer.analyze(user_input);
        std::cout << "Semantic analysis passed." << std::endl;
    }
    catch (const VariableUndefinedError& e) {
        std::cerr << "Semantic Error: " << e.what() << std::endl;
    }

    return 0;
}