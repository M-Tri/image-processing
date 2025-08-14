# Work In Progress ...

# README

## Paths
Ensure paths are updated according to the location of your libraries.

## Features
- Works with integers.
- Designed for high-quality images.
- Handles equations with a consistent pattern.

## Limitations
- **Recognition Accuracy**: The recognition library performs inadequately in recognizing text. Images may need preprocessing.
- **Double Coefficients**: The logic does not handle coefficients with type `double`.
- **Fractions**: Does not work with fractions.

## Notes on Image Processing
The program works with the provided images, but modifications are required for optimal results:
- The '-' and '+' symbols should be properly sized. Specifically, ensure that '-' is short and '+' is small to avoid resembling 't'.
- To improve recognition:
  - Combine every two consecutive '-' symbols into one '-'.
  - Remove 't' from the set of possible characters for extraction.

