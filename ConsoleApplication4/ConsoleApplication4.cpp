#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <stack>
#include <stdexcept>
#include <iostream>

class Token {
public:
    enum class Type { Number, Operator, OpenParen, CloseParen };

    Token(Type type, const std::string& value) : type(type), value(value) {}

    Type getType() const { return type; }
    std::string getValue() const { return value; }

private:
    Type type;
    std::string value;
};



class Lexer {
public:
    static std::vector<Token> tokenize(const std::string& expression) {
        std::vector<Token> tokens;
        for (size_t i = 0; i < expression.length(); ++i) {
            char ch = expression[i];
            if (std::isspace(ch)) continue;

            if (std::isdigit(ch)) {
                std::string num(1, ch);
                while (i + 1 < expression.length() && std::isdigit(expression[i + 1])) {
                    num += expression[++i];
                }
                tokens.emplace_back(Token::Type::Number, num);
            }
            else if (ch == '(') {
                tokens.emplace_back(Token::Type::OpenParen, "(");
            }
            else if (ch == ')') {
                tokens.emplace_back(Token::Type::CloseParen, ")");
            }
            else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
                tokens.emplace_back(Token::Type::Operator, std::string(1, ch));
            }
            else {
                throw std::invalid_argument("Invalid character in expression");
            }
        }
        return tokens;
    }
};



class Parser {
private:
    std::stack<int> operands;        // Pile des opérandes
    std::stack<std::string> operators;  // Pile des opérateurs

    int applyOperator(const std::string& op, int a, int b) {
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/") {
            if (b == 0) throw std::runtime_error("Division by zero");
            return a / b;
        }
        throw std::invalid_argument("Unknown operator");
    }

public:
    int evaluate(const std::vector<Token>& tokens) {
        for (const auto& token : tokens) {
            if (token.getType() == Token::Type::Number) {
                operands.push(std::stoi(token.getValue()));
            }
            else if (token.getType() == Token::Type::Operator) {
                operators.push(token.getValue());
            }
            else if (token.getType() == Token::Type::OpenParen) {
                // Ignorer, car l'analyseur s'en occupe implicitement
            }
            else if (token.getType() == Token::Type::CloseParen) {
                // Lorsqu'on rencontre une parenthèse fermante, résoudre l'expression
                while (!operators.empty() && operators.top() != "(") {
                    int b = operands.top(); operands.pop();
                    int a = operands.top(); operands.pop();
                    std::string op = operators.top(); operators.pop();
                    operands.push(applyOperator(op, a, b));
                }
                if (!operators.empty() && operators.top() == "(") {
                    operators.pop();  // Retirer la parenthèse ouvrante
                }
            }
        }

        // Résoudre toute opération restante
        while (!operators.empty()) {
            int b = operands.top(); operands.pop();
            int a = operands.top(); operands.pop();
            std::string op = operators.top(); operators.pop();
            operands.push(applyOperator(op, a, b));
        }

        return operands.top();
    }
};


int main() {
    std::string expression = "((9+5)+((3+1)+(2+4)))";
    try {
        // Étape 1 : Tokenizer
        std::vector<Token> tokens = Lexer::tokenize(expression);

        // Étape 2 : Parser
        Parser parser;
        int result = parser.evaluate(tokens);

        // Résultat
        std::cout << "Résultat: " << result << std::endl;  // Devrait afficher 24
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }

    return 0;
}
