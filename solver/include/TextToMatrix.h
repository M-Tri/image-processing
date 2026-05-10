#pragma once

#include <string>
#include <vector>

struct LinearSystem {
    std::vector<std::string> equations;
    std::vector<char> variables;
    std::vector<std::vector<double>> A;
    std::vector<double> b;
};

LinearSystem extractInfo(const std::string& text);
