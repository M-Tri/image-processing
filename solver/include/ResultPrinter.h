#pragma once

#include "LinearSystemSolver.h"
#include "TextToMatrix.h"

#include <string>

void printResults(
    const std::string& extractedText,
    const LinearSystem& system,
    const SolveResult& result
);
