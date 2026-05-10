#pragma once

#include "LinearSystemSolver.h"
#include "TextToMatrix.h"

#include <string>
#include <vector>

struct CandidateResult {
    std::string variantName;
    std::string imagePath;
    std::string extractedText;
    LinearSystem system;
    SolveResult solveResult;
    bool success = false;
    std::string errorMessage;
};

struct BestResult {
    CandidateResult candidate;
    int voteCount = 0;
    int totalSuccessfulCandidates = 0;
    std::vector<int> candidateVoteCounts;
    std::vector<std::string> winningVariantNames;
};

BestResult selectBestResult(const std::vector<CandidateResult>& candidates);
