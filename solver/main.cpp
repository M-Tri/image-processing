#include "ImagePreprocessor.h"
#include "LinearSystemSolver.h"
#include "OcrReader.h"
#include "ResultPrinter.h"
#include "ResultSelector.h"
#include "TextToMatrix.h"

#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

bool hasJsonFlag(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--json") {
            return true;
        }
    }

    return false;
}

std::string jsonEscape(const std::string& value) {
    std::ostringstream escaped;

    for (char ch : value) {
        switch (ch) {
            case '"':
                escaped << "\\\"";
                break;
            case '\\':
                escaped << "\\\\";
                break;
            case '\b':
                escaped << "\\b";
                break;
            case '\f':
                escaped << "\\f";
                break;
            case '\n':
                escaped << "\\n";
                break;
            case '\r':
                escaped << "\\r";
                break;
            case '\t':
                escaped << "\\t";
                break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    escaped << "\\u"
                            << std::hex << std::setw(4) << std::setfill('0')
                            << static_cast<int>(static_cast<unsigned char>(ch))
                            << std::dec << std::setfill(' ');
                } else {
                    escaped << ch;
                }
                break;
        }
    }

    return escaped.str();
}

void writeJsonString(std::ostream& output, const std::string& value) {
    output << '"' << jsonEscape(value) << '"';
}

void writeVariablesJson(std::ostream& output, const std::vector<char>& variables) {
    output << '[';
    for (std::size_t i = 0; i < variables.size(); ++i) {
        if (i > 0) {
            output << ',';
        }
        writeJsonString(output, std::string(1, variables[i]));
    }
    output << ']';
}

void writeStringArrayJson(std::ostream& output, const std::vector<std::string>& values) {
    output << '[';
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            output << ',';
        }
        writeJsonString(output, values[i]);
    }
    output << ']';
}

void writeMatrixJson(std::ostream& output, const std::vector<std::vector<double>>& matrix) {
    output << '[';
    for (std::size_t i = 0; i < matrix.size(); ++i) {
        if (i > 0) {
            output << ',';
        }

        output << '[';
        for (std::size_t j = 0; j < matrix[i].size(); ++j) {
            if (j > 0) {
                output << ',';
            }
            output << matrix[i][j];
        }
        output << ']';
    }
    output << ']';
}

void writeVectorJson(std::ostream& output, const std::vector<double>& values) {
    output << '[';
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            output << ',';
        }
        output << values[i];
    }
    output << ']';
}

void writeSolutionJson(
    std::ostream& output,
    const std::vector<char>& variables,
    const Eigen::VectorXd& solution
) {
    output << '{';
    for (int i = 0; i < solution.size() && i < static_cast<int>(variables.size()); ++i) {
        if (i > 0) {
            output << ',';
        }

        writeJsonString(output, std::string(1, variables[i]));
        output << ':' << solution(i);
    }
    output << '}';
}

std::vector<CandidateResult> buildCandidates(
    const std::vector<ImageVariant>& variants,
    const std::string& tessdataPath
) {
    std::vector<CandidateResult> candidates;

    for (const ImageVariant& variant : variants) {
        CandidateResult candidate;
        candidate.variantName = variant.name;
        candidate.imagePath = variant.imagePath;

        try {
            candidate.extractedText = extractTextFromImage(variant.imagePath, tessdataPath);
            candidate.system = extractInfo(candidate.extractedText);
            candidate.solveResult = solveLinearSystem(candidate.system);
            candidate.success = true;
        } catch (const std::exception& error) {
            candidate.errorMessage = error.what();
        }

        candidates.push_back(candidate);
    }

    return candidates;
}

void writeJsonOutput(
    std::ostream& output,
    const BestResult& bestResult,
    const std::vector<ImageVariant>& variants,
    const std::vector<CandidateResult>& candidates
) {
    const CandidateResult& best = bestResult.candidate;
    const double confidence = bestResult.totalSuccessfulCandidates == 0
        ? 0.0
        : static_cast<double>(bestResult.voteCount) /
            static_cast<double>(bestResult.totalSuccessfulCandidates);

    output << std::fixed << std::setprecision(6);

    output << '{';

    output << "\"best\":{";
    output << "\"variantName\":";
    writeJsonString(output, best.variantName);
    output << ",\"imagePath\":";
    writeJsonString(output, best.imagePath);
    output << ",\"voteCount\":" << bestResult.voteCount;
    output << ",\"totalSuccessfulCandidates\":" << bestResult.totalSuccessfulCandidates;
    output << ",\"confidence\":" << confidence;
    output << ",\"winningVariants\":";
    writeStringArrayJson(output, bestResult.winningVariantNames);
    output << ",\"extractedText\":";
    writeJsonString(output, best.extractedText);
    output << ",\"variables\":";
    writeVariablesJson(output, best.system.variables);
    output << ",\"A\":";
    writeMatrixJson(output, best.system.A);
    output << ",\"b\":";
    writeVectorJson(output, best.system.b);
    output << ",\"status\":";
    writeJsonString(output, solutionStatusToString(best.solveResult.status));
    output << ",\"solution\":";
    writeSolutionJson(output, best.system.variables, best.solveResult.solution);
    output << '}';

    output << ",\"variants\":[";
    for (std::size_t i = 0; i < variants.size(); ++i) {
        if (i > 0) {
            output << ',';
        }

        output << "{\"name\":";
        writeJsonString(output, variants[i].name);
        output << ",\"imagePath\":";
        writeJsonString(output, variants[i].imagePath);
        output << '}';
    }
    output << ']';

    output << ",\"candidates\":[";
    for (std::size_t i = 0; i < candidates.size(); ++i) {
        if (i > 0) {
            output << ',';
        }

        const CandidateResult& candidate = candidates[i];

        output << "{\"variantName\":";
        writeJsonString(output, candidate.variantName);
        output << ",\"success\":" << (candidate.success ? "true" : "false");

        if (candidate.success) {
            const int voteCount = i < bestResult.candidateVoteCounts.size()
                ? bestResult.candidateVoteCounts[i]
                : 0;

            output << ",\"voteCount\":" << voteCount;
            output << ",\"extractedText\":";
            writeJsonString(output, candidate.extractedText);
            output << ",\"status\":";
            writeJsonString(output, solutionStatusToString(candidate.solveResult.status));
        } else {
            output << ",\"errorMessage\":";
            writeJsonString(output, candidate.errorMessage);
        }

        output << '}';
    }
    output << ']';

    output << '}';
}

} // namespace

int main(int argc, char* argv[]) {
    const bool jsonMode = hasJsonFlag(argc, argv);
    const std::string imagePath = argc >= 2 ? argv[1] : "equationsImage2.jpg";
    const std::string tessdataPath = argc >= 3 ? argv[2] : "/opt/homebrew/share/tessdata";
    const std::string processedOutputDir = argc >= 4 && std::string(argv[3]) != "--json"
        ? argv[3]
        : "processed";

    try {
        const std::vector<ImageVariant> variants =
            generateImageVariants(imagePath, processedOutputDir);
        const std::vector<CandidateResult> candidates = buildCandidates(variants, tessdataPath);
        const BestResult bestResult = selectBestResult(candidates);

        if (jsonMode) {
            writeJsonOutput(std::cout, bestResult, variants, candidates);
            std::cout << '\n';
        } else {
            std::cout << "Representative variant: " << bestResult.candidate.variantName << '\n';
            std::cout << "Winning variants: ";
            for (std::size_t i = 0; i < bestResult.winningVariantNames.size(); ++i) {
                if (i > 0) {
                    std::cout << ", ";
                }
                std::cout << bestResult.winningVariantNames[i];
            }
            std::cout << '\n';
            std::cout << "Vote count: " << bestResult.voteCount
                      << " of " << bestResult.totalSuccessfulCandidates
                      << " successful candidates\n";

            printResults(
                bestResult.candidate.extractedText,
                bestResult.candidate.system,
                bestResult.candidate.solveResult
            );
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
