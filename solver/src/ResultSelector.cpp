#include "ResultSelector.h"

#include <cstddef>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

std::string formatDouble(double value) {
    std::ostringstream output;
    output << std::fixed << std::setprecision(6) << value;
    return output.str();
}

std::string buildCanonicalKey(const CandidateResult& candidate) {
    std::ostringstream key;

    key << "variables:";
    for (char variable : candidate.system.variables) {
        key << variable << ',';
    }

    key << "|A:";
    for (const std::vector<double>& row : candidate.system.A) {
        key << '[';
        for (double value : row) {
            key << formatDouble(value) << ',';
        }
        key << ']';
    }

    key << "|b:";
    for (double value : candidate.system.b) {
        key << formatDouble(value) << ',';
    }

    key << "|status:" << solutionStatusToString(candidate.solveResult.status);

    return key.str();
}

int statusPreference(SolutionStatus status) {
    return status == SolutionStatus::Unique ? 1 : 0;
}

bool isBetterTieBreaker(const CandidateResult& candidate, const CandidateResult& currentBest) {
    const int candidateStatusPreference = statusPreference(candidate.solveResult.status);
    const int bestStatusPreference = statusPreference(currentBest.solveResult.status);

    if (candidateStatusPreference != bestStatusPreference) {
        return candidateStatusPreference > bestStatusPreference;
    }

    return candidate.system.equations.size() > currentBest.system.equations.size();
}

} // namespace

BestResult selectBestResult(const std::vector<CandidateResult>& candidates) {
    std::unordered_map<std::string, int> voteCounts;
    std::vector<std::string> candidateKeys(candidates.size());

    int totalSuccessfulCandidates = 0;

    for (std::size_t i = 0; i < candidates.size(); ++i) {
        const CandidateResult& candidate = candidates[i];

        if (!candidate.success) {
            continue;
        }

        ++totalSuccessfulCandidates;

        const std::string key = buildCanonicalKey(candidate);
        candidateKeys[i] = key;
        ++voteCounts[key];
    }

    if (totalSuccessfulCandidates == 0) {
        throw std::runtime_error("No successful OCR candidates were produced.");
    }

    bool hasBest = false;
    BestResult best;
    best.candidateVoteCounts.resize(candidates.size(), 0);
    std::string bestKey;

    for (std::size_t i = 0; i < candidates.size(); ++i) {
        const CandidateResult& candidate = candidates[i];

        if (!candidate.success) {
            continue;
        }

        const int voteCount = voteCounts[candidateKeys[i]];
        best.candidateVoteCounts[i] = voteCount;

        if (!hasBest ||
            voteCount > best.voteCount ||
            (voteCount == best.voteCount && isBetterTieBreaker(candidate, best.candidate))) {
            best.candidate = candidate;
            best.voteCount = voteCount;
            bestKey = candidateKeys[i];
            hasBest = true;
        }
    }

    for (std::size_t i = 0; i < candidates.size(); ++i) {
        if (candidates[i].success && candidateKeys[i] == bestKey) {
            best.winningVariantNames.push_back(candidates[i].variantName);
        }
    }

    best.totalSuccessfulCandidates = totalSuccessfulCandidates;
    return best;
}
