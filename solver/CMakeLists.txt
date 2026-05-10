cmake_minimum_required(VERSION 3.16)

project(ocr-equation-solver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc)
find_package(PkgConfig REQUIRED)

pkg_check_modules(TESSERACT REQUIRED IMPORTED_TARGET tesseract)
pkg_check_modules(LEPTONICA REQUIRED IMPORTED_TARGET lept)

add_executable(ocr-equation-solver
    main.cpp
    src/OcrReader.cpp
    src/ImagePreprocessor.cpp
    src/TextToMatrix.cpp
    src/LinearSystemSolver.cpp
    src/ResultSelector.cpp
    src/ResultPrinter.cpp
)

target_include_directories(ocr-equation-solver
    PRIVATE
        include
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(ocr-equation-solver
    PRIVATE
        Eigen3::Eigen
        ${OpenCV_LIBS}
        PkgConfig::TESSERACT
        PkgConfig::LEPTONICA
)
