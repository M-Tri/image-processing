#include "OcrReader.h"

#include <allheaders.h>
#include <tesseract/baseapi.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace {

struct PixDeleter {
    void operator()(Pix* image) const {
        if (image != nullptr) {
            pixDestroy(&image);
        }
    }
};

struct TessEndGuard {
    explicit TessEndGuard(tesseract::TessBaseAPI& api) : api(api) {}

    ~TessEndGuard() {
        api.End();
    }

    tesseract::TessBaseAPI& api;
};

} // namespace

std::string extractTextFromImage(
    const std::string& imagePath,
    const std::string& tessdataPath
) {
    tesseract::TessBaseAPI tess;

    if (tess.Init(tessdataPath.c_str(), "eng") != 0) {
        throw std::runtime_error("Could not initialize Tesseract.");
    }

    TessEndGuard tessEndGuard(tess);

    std::unique_ptr<Pix, PixDeleter> image(pixRead(imagePath.c_str()));

    if (!image) {
        throw std::runtime_error("Could not read image file: " + imagePath);
    }

    tess.SetImage(image.get());

    std::unique_ptr<char[]> output(tess.GetUTF8Text());

    if (!output) {
        throw std::runtime_error("Could not extract text from image.");
    }

    return std::string(output.get());
}
