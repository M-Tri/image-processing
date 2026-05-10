#include "TextToMatrix.h"

#include <cctype>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Expression {
    std::map<char, double> coefficients;
    double constant = 0.0;
};

struct ParsedEquation {
    std::string original;
    std::map<char, double> coefficients;
    double rhs = 0.0;
};

std::string removeWhitespace(const std::string& input) {
    std::string result;

    for (char ch : input) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            result.push_back(ch);
        }
    }

    return result;
}

std::string removeAsterisks(const std::string& input) {
    std::string result;

    for (char ch : input) {
        if (ch != '*') {
            result.push_back(ch);
        }
    }

    return result;
}

std::string normalizeEquation(const std::string& input) {
    return removeAsterisks(removeWhitespace(input));
}

std::vector<std::string> splitTerms(const std::string& expression) {
    std::vector<std::string> terms;

    if (expression.empty()) {
        return terms;
    }

    std::string normalized = expression;

    if (normalized[0] != '+' && normalized[0] != '-') {
        normalized = "+" + normalized;
    }

    std::size_t start = 0;

    for (std::size_t i = 1; i < normalized.size(); ++i) {
        if (normalized[i] == '+' || normalized[i] == '-') {
            terms.push_back(normalized.substr(start, i - start));
            start = i;
        }
    }

    terms.push_back(normalized.substr(start));
    return terms;
}

bool containsVariable(const std::string& term, std::size_t& variablePosition) {
    for (std::size_t i = 0; i < term.size(); ++i) {
        if (std::isalpha(static_cast<unsigned char>(term[i]))) {
            variablePosition = i;
            return true;
        }
    }

    return false;
}

double parseNumber(const std::string& numberText) {
    if (numberText.empty()) {
        return 1.0;
    }

    if (numberText == "+") {
        return 1.0;
    }

    if (numberText == "-") {
        return -1.0;
    }

    try {
        return std::stod(numberText);
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid number: " + numberText);
    }
}

Expression parseExpression(const std::string& expressionText) {
    Expression expression;

    std::vector<std::string> terms = splitTerms(expressionText);

    for (const std::string& term : terms) {
        if (term.empty()) {
            continue;
        }

        std::size_t variablePosition = std::string::npos;

        if (containsVariable(term, variablePosition)) {
            char variable = static_cast<char>(
                std::tolower(static_cast<unsigned char>(term[variablePosition]))
            );

            std::string coefficientText = term.substr(0, variablePosition);
            std::string afterVariable = term.substr(variablePosition + 1);

            if (!afterVariable.empty()) {
                throw std::invalid_argument(
                    "Invalid term. Only simple linear terms are supported: " + term
                );
            }

            double coefficient = parseNumber(coefficientText);
            expression.coefficients[variable] += coefficient;
        } else {
            double constant = parseNumber(term);
            expression.constant += constant;
        }
    }

    return expression;
}

ParsedEquation parseEquation(const std::string& line) {
    std::string equation = normalizeEquation(line);

    std::size_t equalsPosition = equation.find('=');

    if (equalsPosition == std::string::npos) {
        throw std::invalid_argument("Equation is missing '=': " + line);
    }

    if (equation.find('=', equalsPosition + 1) != std::string::npos) {
        throw std::invalid_argument("Equation has more than one '=': " + line);
    }

    std::string leftText = equation.substr(0, equalsPosition);
    std::string rightText = equation.substr(equalsPosition + 1);

    if (leftText.empty() || rightText.empty()) {
        throw std::invalid_argument("Equation has an empty side: " + line);
    }

    Expression left = parseExpression(leftText);
    Expression right = parseExpression(rightText);

    ParsedEquation parsed;
    parsed.original = line;

    for (const auto& [variable, coefficient] : left.coefficients) {
        parsed.coefficients[variable] += coefficient;
    }

    for (const auto& [variable, coefficient] : right.coefficients) {
        parsed.coefficients[variable] -= coefficient;
    }

    parsed.rhs = right.constant - left.constant;

    return parsed;
}

std::vector<char> buildVariableOrder(const std::set<char>& variables) {
    std::vector<char> orderedVariables;

    const std::string preferredOrder = "xyz";

    for (char variable : preferredOrder) {
        if (variables.count(variable)) {
            orderedVariables.push_back(variable);
        }
    }

    for (char variable : variables) {
        if (preferredOrder.find(variable) == std::string::npos) {
            orderedVariables.push_back(variable);
        }
    }

    return orderedVariables;
}

} // namespace

LinearSystem extractInfo(const std::string& text) {
    std::istringstream input(text);
    std::string line;

    std::vector<ParsedEquation> parsedEquations;
    std::set<char> variableSet;

    while (std::getline(input, line)) {
        std::string cleanedLine = removeWhitespace(line);

        if (cleanedLine.empty()) {
            continue;
        }

        ParsedEquation parsed = parseEquation(line);
        parsedEquations.push_back(parsed);

        for (const auto& [variable, coefficient] : parsed.coefficients) {
            variableSet.insert(variable);
        }
    }

    if (parsedEquations.empty()) {
        throw std::invalid_argument("No equations were found in the OCR text.");
    }

    if (variableSet.empty()) {
        throw std::invalid_argument("No variables were found in the equations.");
    }

    LinearSystem system;
    system.variables = buildVariableOrder(variableSet);

    for (const ParsedEquation& parsed : parsedEquations) {
        system.equations.push_back(parsed.original);

        std::vector<double> row;

        for (char variable : system.variables) {
            auto it = parsed.coefficients.find(variable);

            if (it == parsed.coefficients.end()) {
                row.push_back(0.0);
            } else {
                row.push_back(it->second);
            }
        }

        system.A.push_back(row);
        system.b.push_back(parsed.rhs);
    }

    return system;
}
