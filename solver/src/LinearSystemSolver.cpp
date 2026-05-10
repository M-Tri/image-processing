#include "LinearSystemSolver.h"

#include <Eigen/Dense>

#include <cstddef>
#include <stdexcept>
#include <string>

namespace {

void validateDimensions(const LinearSystem& system) {
    const std::size_t rows = system.A.size();
    const std::size_t cols = system.variables.size();

    if (system.b.size() != rows) {
        throw std::runtime_error("Inconsistent matrix dimensions: b size does not match A rows.");
    }

    for (std::size_t i = 0; i < rows; ++i) {
        if (system.A[i].size() != cols) {
            throw std::runtime_error("Inconsistent matrix dimensions: A row size does not match variables.");
        }
    }
}

SolutionStatus classifySystem(const Eigen::MatrixXd& A, const Eigen::VectorXd& b) {
    const int rows = static_cast<int>(A.rows());
    const int cols = static_cast<int>(A.cols());

    Eigen::MatrixXd augmented(rows, cols + 1);
    augmented.leftCols(cols) = A;
    augmented.col(cols) = b;

    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qrA(A);
    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qrAugmented(augmented);

    const int rankA = qrA.rank();
    const int rankAugmented = qrAugmented.rank();

    if (rankA < rankAugmented) {
        return SolutionStatus::None;
    }

    if (rankA < cols) {
        return SolutionStatus::Infinite;
    }

    return SolutionStatus::Unique;
}

} // namespace

SolveResult solveLinearSystem(const LinearSystem& system) {
    validateDimensions(system);

    const int rows = static_cast<int>(system.A.size());
    const int cols = static_cast<int>(system.variables.size());

    SolveResult result;
    result.A.resize(rows, cols);
    result.b.resize(rows);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result.A(i, j) = system.A[i][j];
        }

        result.b(i) = system.b[i];
    }

    result.status = classifySystem(result.A, result.b);

    if (result.status == SolutionStatus::Unique || result.status == SolutionStatus::Infinite) {
        Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr(result.A);
        result.solution = qr.solve(result.b);
    } else {
        result.solution.resize(0);
    }

    return result;
}

std::string solutionStatusToString(SolutionStatus status) {
    switch (status) {
        case SolutionStatus::Unique:
            return "Unique solution";
        case SolutionStatus::Infinite:
            return "Infinitely many solutions";
        case SolutionStatus::None:
            return "No solution";
        default:
            return "Unknown";
    }
}
