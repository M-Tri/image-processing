#pragma once

#include <string>
#include <vector>

struct ImageVariant {
    std::string name;
    std::string imagePath;
};

std::vector<ImageVariant> generateImageVariants(
    const std::string& originalImagePath,
    const std::string& outputDirectory
);
