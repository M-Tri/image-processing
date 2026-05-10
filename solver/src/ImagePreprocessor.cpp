#include "ImagePreprocessor.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string buildVariantPath(
    const std::filesystem::path& outputDirectory,
    const std::string& variantName
) {
    return (outputDirectory / (variantName + ".png")).string();
}

void writeImageOrThrow(const std::string& path, const cv::Mat& image) {
    if (!cv::imwrite(path, image)) {
        throw std::runtime_error("Could not write processed image: " + path);
    }
}

void addVariant(
    std::vector<ImageVariant>& variants,
    const std::filesystem::path& outputDirectory,
    const std::string& name,
    const cv::Mat& image
) {
    const std::string path = buildVariantPath(outputDirectory, name);
    writeImageOrThrow(path, image);
    variants.push_back({name, path});
}

cv::Mat makeContrastEnhanced(const cv::Mat& grayscale) {
    cv::Mat enhanced;
    cv::equalizeHist(grayscale, enhanced);
    return enhanced;
}

cv::Mat makeSharpened(const cv::Mat& grayscale) {
    cv::Mat sharpened;
    cv::Mat kernel = (
        cv::Mat_<float>(3, 3) << 0, -1, 0,
                                -1, 5, -1,
                                 0, -1, 0
    );
    cv::filter2D(grayscale, sharpened, grayscale.depth(), kernel);
    return sharpened;
}

} // namespace

std::vector<ImageVariant> generateImageVariants(
    const std::string& originalImagePath,
    const std::string& outputDirectory
) {
    const cv::Mat original = cv::imread(originalImagePath, cv::IMREAD_COLOR);

    if (original.empty()) {
        throw std::runtime_error("Could not read original image file: " + originalImagePath);
    }

    const std::filesystem::path outputPath(outputDirectory);
    std::filesystem::create_directories(outputPath);

    std::vector<ImageVariant> variants;

    addVariant(variants, outputPath, "original", original);

    cv::Mat grayscale;
    cv::cvtColor(original, grayscale, cv::COLOR_BGR2GRAY);
    addVariant(variants, outputPath, "grayscale", grayscale);

    cv::Mat resized;
    cv::resize(grayscale, resized, cv::Size(), 2.0, 2.0, cv::INTER_CUBIC);
    addVariant(variants, outputPath, "resized_2x", resized);

    cv::Mat binaryThreshold;
    cv::threshold(grayscale, binaryThreshold, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    addVariant(variants, outputPath, "binary_threshold", binaryThreshold);

    cv::Mat adaptiveThreshold;
    cv::adaptiveThreshold(
        grayscale,
        adaptiveThreshold,
        255,
        cv::ADAPTIVE_THRESH_GAUSSIAN_C,
        cv::THRESH_BINARY,
        31,
        11
    );
    addVariant(variants, outputPath, "adaptive_threshold", adaptiveThreshold);

    cv::Mat invertedGrayscale;
    cv::bitwise_not(grayscale, invertedGrayscale);
    addVariant(variants, outputPath, "inverted_grayscale", invertedGrayscale);

    cv::Mat contrastEnhanced = makeContrastEnhanced(grayscale);
    addVariant(variants, outputPath, "contrast_enhanced", contrastEnhanced);

    cv::Mat sharpened = makeSharpened(grayscale);
    addVariant(variants, outputPath, "sharpened", sharpened);

    cv::Mat blurred;
    cv::GaussianBlur(grayscale, blurred, cv::Size(3, 3), 0);

    cv::Mat blurThenThreshold;
    cv::threshold(blurred, blurThenThreshold, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    addVariant(variants, outputPath, "blur_then_threshold", blurThenThreshold);

    return variants;
}
