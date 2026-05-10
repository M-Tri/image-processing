#pragma once

#include "TextToMatrix.h"

#include <Eigen/Dense>
#include <string>

enum class SolutionStatus {
    Unique,
    Infinite,
    None
};

struct SolveResult {
    SolutionStatus status;
    Eigen::MatrixXd A;
    Eigen::VectorXd b;
    Eigen::VectorXd solution;
};

SolveResult solveLinearSystem(const LinearSystem& system);

std::string solutionStatusToString(SolutionStatus status);
