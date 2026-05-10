#include "ResultPrinter.h"

#include <iostream>
#include <string>

void printResults(
    const std::string& extractedText,
    const LinearSystem& system,
    const SolveResult& result
) {
    std::cout << '\n';

    std::cout << "Extracted Text:\n";
    std::cout << extractedText << '\n';

    std::cout << "Detected Variables:\n";
    for (char variable : system.variables) {
        std::cout << variable << ' ';
    }
    std::cout << "\n\n";

    std::cout << "Parsed Equations:\n";
    for (const std::string& equation : system.equations) {
        std::cout << equation << '\n';
    }
    std::cout << '\n';

    std::cout << "Matrix A:\n";
    std::cout << result.A << "\n\n";

    std::cout << "Vector b:\n";
    std::cout << result.b << "\n\n";

    std::cout << "System status:\n";
    std::cout << solutionStatusToString(result.status) << "\n\n";

    if (result.status == SolutionStatus::None) {
        std::cout << "The equations are inconsistent, so there is no solution.\n\n";
        return;
    }

    if (result.status == SolutionStatus::Infinite) {
        std::cout << "The system has infinitely many solutions.\n";
        std::cout << "This version does not yet output the full parametric solution.\n";
        std::cout << "One possible solution returned by Eigen is:\n\n";
    } else {
        std::cout << "Solution for A * x = b:\n";
    }

    for (int i = 0; i < result.solution.size(); ++i) {
        std::cout << system.variables[i] << " = " << result.solution(i) << '\n';
    }

    std::cout << '\n';
}
